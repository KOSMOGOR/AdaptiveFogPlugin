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

static TAutoConsoleVariable<float3> CVarFogColor(
	TEXT("r.AFog.FogColor"),
	float3(0.9f, 0.9f, 0.9f));

static TAutoConsoleVariable<float> CVarMaxFogFactor(
	TEXT("r.AFog.MaxFogFactor"),
	0.8f);

static TAutoConsoleVariable<float> CVarFogCurve(
	TEXT("r.AFog.FogCurve"),
	1.5f);

static TAutoConsoleVariable<float> CVarFogStart(
	TEXT("r.AFog.FogStart"),
	0.05f);

static TAutoConsoleVariable<float> CVarBloomThreshold(
	TEXT("r.AFog.BloomThreshold"),
	10.25f);

static TAutoConsoleVariable<float> CVarBloomPower(
	TEXT("r.AFog.BloomPower"),
	10f);

static TAutoConsoleVariable<float> CVarBloomWidth(
	TEXT("r.AFog.BloomWidth"),
	0.2f);


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
	
	FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	TShaderMapRef<FAdaptiveFogVS> ScreenPassVS(GlobalShaderMap);
	TShaderMapRef<FAdaptiveFogPS> ScreenPassPS(GlobalShaderMap);

	FAdaptiveFogPS::FParameters* Parameters = GraphBuilder.AllocParameters<FAdaptiveFogPS::FParameters>();
	Parameters->View = View.ViewUniformBuffer;
	Parameters->SceneTexturesStruct = Inputs.SceneTextures;
	Parameters->FogColor = CVarFogColor->GetFloat3();
	Parameters->MaxFogFactor = CVarMaxFogFactor->GetFloat();
	Parameters->FogCurve = CVarFogCurve->GetFloat();
	Parameters->FogStart = CVarFogStart->GetFloat();
	Parameters->BloomThreshold = CVarBloomThreshold->GetFloat();
	Parameters->BloomPower = CVarBloomPower->GetFloat();
	Parameters->BloomWidth = CVarBloomWidth->GetFloat();

	Parameters->RenderTargets[0] = ResultRenderTarget.GetRenderTargetBinding();

	AddDrawScreenPass(
		GraphBuilder,
		RDG_EVENT_NAME("AdaptiveFogShader"),
		View,
		Viewport,
		Viewport,
		ScreenPassVS,
		ScreenPassPS,
		Parameters
	);

	AddCopyTexturePass(GraphBuilder, ResultTexture, SceneColor.Texture);
}