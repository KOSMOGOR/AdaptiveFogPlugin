#include "AdaptiveFogSceneViewExtension.h"
#include "AdaptiveFogShaders.h"

#include "FXRenderingUtils.h"
#include "PostProcess/PostProcessInputs.h"
#include "DynamicResolutionState.h"


static TAutoConsoleVariable<int32> CVarEnabled(
	TEXT("r.AFog"),
	1,
	TEXT("Controls Full Screen Pass plugin\n")
	TEXT(" 0: disabled\n")
	TEXT(" 1: enabled (default)"));

static TAutoConsoleVariable<float> CVarMaxFogFactor(
	TEXT("r.AFog.MaxFogFactor"),
	0.8f,
	TEXT("Description"));

static TAutoConsoleVariable<float> CVarFogCurve(
	TEXT("r.AFog.FogCurve"),
	1.5f,
	TEXT("Description"));

static TAutoConsoleVariable<float> CVarFogStart(
	TEXT("r.AFog.FogStart"),
	0.05f,
	TEXT("Description"));

static TAutoConsoleVariable<float> CVarBloomThreshold(
	TEXT("r.AFog.BloomThreshold"),
	10.25f,
	TEXT("Description"));

static TAutoConsoleVariable<float> CVarBloomPower(
	TEXT("r.AFog.BloomPower"),
	10.0f,
	TEXT("Description"));

static TAutoConsoleVariable<float> CVarBloomWidth(
	TEXT("r.AFog.BloomWidth"),
	0.2f,
	TEXT("Description"));


FAdaptiveFogSceneViewExtension::FAdaptiveFogSceneViewExtension(const FAutoRegister& AutoRegister) :
	FSceneViewExtensionBase(AutoRegister)
{

}

void FAdaptiveFogSceneViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs)
{
	if (CVarEnabled->GetInt() == 0)
	{
		return;
	}

	Inputs.Validate();

	const FIntRect PrimaryViewRect = UE::FXRenderingUtils::GetRawViewRectUnsafe(View);

	FScreenPassTexture SceneColor((*Inputs.SceneTextures)->SceneColorTexture, PrimaryViewRect);

	if (!SceneColor.IsValid())
	{
		return;
	}

	const FScreenPassTextureViewport Viewport(SceneColor);

	FRDGTextureDesc SceneColorDesc = SceneColor.Texture->Desc;

	SceneColorDesc.Format = PF_FloatRGBA;
	FLinearColor ClearColor(0., 0., 0., 0.);
	SceneColorDesc.ClearValue = FClearValueBinding(ClearColor);

	FRDGTexture* ResultTexture = GraphBuilder.CreateTexture(SceneColorDesc, TEXT("FulllScreenPassResult"));
	FScreenPassRenderTarget ResultRenderTarget = FScreenPassRenderTarget(ResultTexture, SceneColor.ViewRect, ERenderTargetLoadAction::EClear);

	FRDGTexture* BloomTexture = GraphBuilder.CreateTexture(SceneColorDesc, TEXT("FulllScreenPassBloom"));
	FScreenPassRenderTarget BloomRenderTarget = FScreenPassRenderTarget(BloomTexture, SceneColor.ViewRect, ERenderTargetLoadAction::EClear);
	
	FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	TShaderMapRef<FAdaptiveFogVS> ScreenPassVS(GlobalShaderMap);
	TShaderMapRef<FAdaptiveFogPSBloom> ScreenPassPSBloom(GlobalShaderMap);
	TShaderMapRef<FAdaptiveFogPSFinal> ScreenPassPSFinal(GlobalShaderMap);

	FAdaptiveFogPSBloom::FParameters* ParametersBloom = GraphBuilder.AllocParameters<FAdaptiveFogPSBloom::FParameters>();
	ParametersBloom->View = View.ViewUniformBuffer;
	ParametersBloom->SceneTexturesStruct = Inputs.SceneTextures;
	ParametersBloom->BloomThreshold = CVarBloomThreshold->GetFloat();
	ParametersBloom->BloomPower = CVarBloomPower->GetFloat();
	ParametersBloom->BloomWidth = CVarBloomWidth->GetFloat();
	ParametersBloom->RenderTargets[0] = BloomRenderTarget.GetRenderTargetBinding();

	FAdaptiveFogPSFinal::FParameters* ParametersFinal = GraphBuilder.AllocParameters<FAdaptiveFogPSFinal::FParameters>();
	ParametersFinal->View = View.ViewUniformBuffer;
	ParametersFinal->SceneTexturesStruct = Inputs.SceneTextures;
	ParametersFinal->FogColor = FVector3f(0.9f, 0.9f, 0.9f);
	ParametersFinal->MaxFogFactor = CVarMaxFogFactor->GetFloat();
	ParametersFinal->FogCurve = CVarFogCurve->GetFloat();
	ParametersFinal->FogStart = CVarFogStart->GetFloat();
	ParametersFinal->BloomTexture = BloomTexture;
	ParametersFinal->BloomSampler = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	ParametersFinal->RenderTargets[0] = ResultRenderTarget.GetRenderTargetBinding();


	AddDrawScreenPass(
		GraphBuilder,
		RDG_EVENT_NAME("AdaptiveFogShaderBloom"),
		View,
		Viewport,
		Viewport,
		ScreenPassVS,
		ScreenPassPSBloom,
		ParametersBloom
	);

	AddDrawScreenPass(
		GraphBuilder,
		RDG_EVENT_NAME("AdaptiveFogShaderBloom"),
		View,
		Viewport,
		Viewport,
		ScreenPassVS,
		ScreenPassPSFinal,
		ParametersFinal
	);

	AddCopyTexturePass(GraphBuilder, ResultTexture, SceneColor.Texture);
}