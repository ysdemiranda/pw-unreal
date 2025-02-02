// Copyright Epic Games, Inc. All Rights Reserved.

#include "unrealpw.h"

#include "Misc/MessageDialog.h"
#include "CoreGlobals.h"
#include "pipewire/pipewire.h"
#include <string.h>

#define LOCTEXT_NAMESPACE "FunrealpwModule"

DEFINE_LOG_CATEGORY(LogPipeWire)

void FunrealpwModule::StartupModule()
{
#if (!PLATFORM_LINUX) // We're not into that kinky stuff
	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", 
	"You are trying to load the PipeWire interface" \
	" plugin on a non-linux system, weirdo :/"));

	UE_LOG(LogPipeWire, Warning, TEXT("Skipped initializing libpipewire, this is not a linux OS!"));

	return;
#endif

	auto argv = FCommandLine::GetOriginal();
	FTCHARToUTF8 argv_c(argv);
	auto argv_x = argv_c.Get();
	auto argv_x2 = &argv_x;

	auto tokens = TArray<FString>();
	auto switches = TArray<FString>();
	FCommandLine::Parse(argv, tokens, switches);
	int argc = tokens.Num();

	UE_LOG(LogPipeWire, Log, TEXT("Initializing libpipewire... "));

	pw_init(&argc, (char***)&argv_x2);
	_bPwLoaded = true;

	UE_LOG(LogPipeWire, Log, TEXT("Linked with libpipewire %hs"), pw_get_library_version());
	UE_LOG(LogPipeWire, Log, TEXT("Compiled with libpipewire %hs"), pw_get_headers_version());
}

void FunrealpwModule::ShutdownModule()
{
	if(_bPwLoaded)
	{
		UE_LOG(LogPipeWire, Log, TEXT("Deinitializing libpipewire... "));
		pw_deinit();
		_bPwLoaded = false;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FunrealpwModule, unrealpw)