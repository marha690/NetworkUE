// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainTileActor.h"
#include "TerrainMeshProvider.h"
#include "Net/UnrealNetwork.h"

ATerrainTileActor::ATerrainTileActor()
	:Material(nullptr)
{

}

void ATerrainTileActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetReplicates(true);
	bAlwaysRelevant = false;
	bNetUseOwnerRelevancy = true;
	bOnlyRelevantToOwner = false;
	NetCullDistanceSquared = 2000*2000;

	float TileSize = 1000.f;
	int TileSubdivision = 10;
	GenerateMesh(TileSize, TileSubdivision);
}

void ATerrainTileActor::GenerateMesh(const float TileSize, const int Subdivision)
{
	UTerrainMeshProvider* Provider = NewObject<UTerrainMeshProvider>(this, TEXT("RuntimeMeshProvider-Box"));
	if (Provider)
	{
		Provider->SetBoxMaterial(Material);
		Provider->SetTerrainInfo(TileSize, Subdivision);

		GetRuntimeMeshComponent()->Initialize(Provider);
	}
}



