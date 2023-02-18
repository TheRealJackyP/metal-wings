#pragma once

#include "Delegates/Delegate.h"

struct FAssetData;
class FMessageEndpoint;
class UBlueprint;

class RIDERBLUEPRINT_API BluePrintProvider
{
public:
	static void AddAsset(const FAssetData& AssetData);

	static bool IsBlueprint(const FString& pathName);

	static void OpenBlueprint(const FString& path,
	                          const TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe>&
	                          messageEndpoint);
};
