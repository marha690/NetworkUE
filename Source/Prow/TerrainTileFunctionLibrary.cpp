// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainTileFunctionLibrary.h"
#include "TerrainSubsystem.h"

void UTerrainTileFunctionLibrary::GetTileSize(float& Size)
{
	Size = INTERNAL_TERRAIN_INFO::TileSize;
}

void UTerrainTileFunctionLibrary::GetTileIndex(const FVector Position, int& X, int& Y)
{
	bool IsPositiveX = Position.X > 0;
	bool IsPositiveY = Position.Y > 0;

	float HalfTileSize = INTERNAL_TERRAIN_INFO::TileSize * 0.5f;
	int XX = (abs(Position.X) + HalfTileSize) / INTERNAL_TERRAIN_INFO::TileSize;
	int YY = (abs(Position.Y) + HalfTileSize) / INTERNAL_TERRAIN_INFO::TileSize;

	X = IsPositiveX ? XX : -XX;
	Y = IsPositiveY ? YY : -YY;
}

void UTerrainTileFunctionLibrary::GetTilePosition(const int X, const int Y, FVector &Position)
{
	bool IsPositiveX = X > 0;
	bool IsPositiveY = Y > 0;

	float XX = X * INTERNAL_TERRAIN_INFO::TileSize;
	float YY = Y * INTERNAL_TERRAIN_INFO::TileSize;

	Position = FVector(XX, YY, 0);
}