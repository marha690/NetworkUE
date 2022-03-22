// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TerrainSubsystem.generated.h"

namespace INTERNAL_TERRAIN_INFO {
	static float TileSize = 1000;
	static int Subdvision = 3;
}

class ATerrainTileActor;

/**
 * 
 */
UCLASS()
class PROW_API UTerrainSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// End USubsystem

	UFUNCTION(BlueprintCallable)
	ATerrainTileActor* SpawnTile(const int X, const int Y);
	//void RemoveTiles(const int X, const int Y);
private:
	//Stores the tiles which exists
	TDoubleLinkedList<TPair<FIntPoint, ATerrainTileActor*>> LoadedTiles;
	
};
