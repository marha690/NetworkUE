// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TerrainTileFunctionLibrary.generated.h"




/**
 * 
 */
UCLASS()
class PROW_API UTerrainTileFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	static void GetTileSize(float& Size);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
	static void GetTileIndex(const FVector Position, int& X, int& Y);

	UFUNCTION(BlueprintCallable, Category = "Terrain")
		static void GetTilePosition(const int X, const int Y, FVector& Position);
};