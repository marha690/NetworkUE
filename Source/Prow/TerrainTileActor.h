// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshActor.h"
#include "TerrainTileActor.generated.h"

/**
 * 
 */
UCLASS()
class PROW_API ATerrainTileActor : public ARuntimeMeshActor
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

public:
	ATerrainTileActor();
	void OnConstruction(const FTransform& Transform) override;

	void GenerateMesh(const float TileSize, const int Subdivision);

private:

	FBoxSphereBounds TileBoundaries;
	FVector TileCenter;
};
