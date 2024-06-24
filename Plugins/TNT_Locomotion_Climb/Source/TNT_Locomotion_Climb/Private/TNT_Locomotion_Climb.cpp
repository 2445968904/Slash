// Copyright TNT,2022. All Rights Reserved.

#include "TNT_Locomotion_Climb.h"

#define LOCTEXT_NAMESPACE "FTNT_Locomotion_ClimbModule"

void FTNT_Locomotion_ClimbModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FTNT_Locomotion_ClimbModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTNT_Locomotion_ClimbModule, TNT_Locomotion_Climb)