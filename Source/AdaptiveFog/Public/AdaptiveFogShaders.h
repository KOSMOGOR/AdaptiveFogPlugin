#pragma once
#include "CoreMinimal.h"
#include "ScreenPass.h"
#include "SceneTexturesConfig.h"


class FAdaptiveFogVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FAdaptiveFogVS);

	FAdaptiveFogVS() = default;
	FAdaptiveFogVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer) {}
};

class FAdaptiveFogPSBloom : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FAdaptiveFogPSBloom);
	SHADER_USE_PARAMETER_STRUCT(FAdaptiveFogPSBloom, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		RENDER_TARGET_BINDING_SLOTS()
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTexturesStruct)
		SHADER_PARAMETER(float, BloomThreshold)
		SHADER_PARAMETER(float, BloomPower)
		SHADER_PARAMETER(float, BloomWidth)
	END_SHADER_PARAMETER_STRUCT()
};

class FAdaptiveFogPSFinal : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FAdaptiveFogPSFinal);
	SHADER_USE_PARAMETER_STRUCT(FAdaptiveFogPSFinal, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		RENDER_TARGET_BINDING_SLOTS()
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTexturesStruct)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BloomTexture)
		SHADER_PARAMETER(FVector3f, FogColor)
		SHADER_PARAMETER(float, MaxFogFactor)
		SHADER_PARAMETER(float, FogCurve)
		SHADER_PARAMETER(float, FogStart)
	END_SHADER_PARAMETER_STRUCT()
};