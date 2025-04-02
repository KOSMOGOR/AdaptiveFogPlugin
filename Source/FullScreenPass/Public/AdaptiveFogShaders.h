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

class FAdaptiveFogPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FAdaptiveFogPS);
	SHADER_USE_PARAMETER_STRUCT(FAdaptiveFogPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		RENDER_TARGET_BINDING_SLOTS()
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTexturesStruct)
		SHADER_PARAMETER(float3, FogColor)
		SHADER_PARAMETER(float, MaxFogFactor)
		SHADER_PARAMETER(float, FogCurve)
		SHADER_PARAMETER(float, FogStart)
		SHADER_PARAMETER(float, BloomThreshold)
		SHADER_PARAMETER(float, BloomPower)
		SHADER_PARAMETER(float, BloomWidth)
	END_SHADER_PARAMETER_STRUCT()
};