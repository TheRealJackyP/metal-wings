#pragma once

#include "CoreMinimal.h"

/**
 * @brief A class with a few static utilities to make dealing with Viewports a little easier.
 */
METALWINGSCORE_API class FViewportUtilities
{
public:
	/**
    * @brief Gets the current size of the game's viewport. 
    * @return A vector containing the x and y values representing the current size of the viewport.
    */
	METALWINGSCORE_API static FVector2D GetGameViewportSize();
};
