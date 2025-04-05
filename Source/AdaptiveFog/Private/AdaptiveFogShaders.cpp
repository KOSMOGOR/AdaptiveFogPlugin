#include "AdaptiveFogShaders.h"

IMPLEMENT_GLOBAL_SHADER(FAdaptiveFogVS, "/AdaptiveFog/Private/AdaptiveFogShader.usf", "MainVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FAdaptiveFogPSBloom, "/AdaptiveFog/Private/AdaptiveFogShader.usf", "PS_Otis_AFG_PerformBloom", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FAdaptiveFogPSFinal, "/AdaptiveFog/Private/AdaptiveFogShader.usf", "PS_Otis_AFG_BlendFogWithNormalBuffer", SF_Pixel);