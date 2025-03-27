#include "AdaptiveFogShaders.h"

IMPLEMENT_GLOBAL_SHADER(FAdaptiveFogVS, "/AdaptiveFog/Private/AdaptiveFogShader.usf", "MainVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FAdaptiveFogPS, "/AdaptiveFog/Private/AdaptiveFogShader.usf", "MainPS", SF_Pixel);