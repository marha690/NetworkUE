// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainSubsystem.h"
#include "TerrainTileFunctionLibrary.h"
#include "TerrainTileActor.h"
#include "Net/UnrealNetwork.h"

void UTerrainSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UTerrainSubsystem::Deinitialize()
{
}

bool UTerrainSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer)) {
		return false;
	}
	return true;
}

ATerrainTileActor* UTerrainSubsystem::SpawnTile(const int X, const int Y)
{
	FVector Position;
	UTerrainTileFunctionLibrary::GetTilePosition(X, Y, Position);
	FIntPoint NewPoint = FIntPoint(X, Y);

	// Find existing if terrain has already been created
	auto Temp = LoadedTiles.GetHead();
	while (Temp != LoadedTiles.GetTail()) {

		if (Temp->GetValue().Key == NewPoint) {
			return Temp->GetValue().Value;
		}
		Temp = Temp->GetNextNode();
	}

	// Create terrain
	ATerrainTileActor* Terrain = (ATerrainTileActor*)GetWorld()->SpawnActor(ATerrainTileActor::StaticClass(), &Position);
	LoadedTiles.AddTail(TPair<FIntPoint, ATerrainTileActor*>(FIntPoint(X, Y), Terrain));
	return Terrain;
}
