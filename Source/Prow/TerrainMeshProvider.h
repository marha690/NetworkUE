// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshProvider.h"
#include "TerrainMeshProvider.generated.h"


/**
 * 
 */
UCLASS(HideCategories = Object, BlueprintType)
class PROW_API UTerrainMeshProvider : public URuntimeMeshProvider
{
	GENERATED_BODY()
	
private:
	mutable FCriticalSection PropertySyncRoot;

	UPROPERTY(Category = "RuntimeMesh|Providers|Terrain", VisibleAnywhere, BlueprintGetter = GetBoxMaterial, BlueprintSetter = SetBoxMaterial)
		UMaterialInterface* Material;

	// The size this tile has
	float TileSize;
	int Subdivision = 1;
	int PointsPerLine = Subdivision + 1;
	int NumberOfPoints = PointsPerLine * PointsPerLine;

	// Terrain point information, each point can be adjusted in height.
	TArray<FVector2D> PointPosition;
	TArray<float> DefaultHeight;
	TArray<float> CurrentHeight;
	const float AllowedHeightDifference = 400.0f;

public:
	UFUNCTION(Category = "RuntimeMesh|Providers|Terrain", BlueprintCallable)
		UMaterialInterface* GetBoxMaterial() const;
	UFUNCTION(Category = "RuntimeMesh|Providers|Terrain", BlueprintCallable)
		void SetBoxMaterial(UMaterialInterface* InMaterial);

	UFUNCTION()
		bool UpdateHeight(const int X, const int Y, const float HeightModification);
		bool UpdateHeight(const int Index, const float HeightModification);
		FVector GetTerrainPointPosition(const int Index);

	UFUNCTION()
		void SetTerrainInfo(float Size, int SubdivisionLevel);

	// returns the linear index of a 2D array.
	inline int LinearIndex(const int X, const int Y);
	inline bool CubicIndex(const int LinearIndex, int& X, int& Y);

protected:
	void Initialize() override;
	FBoxSphereBounds GetBounds() override;
	bool GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData) override;
	FRuntimeMeshCollisionSettings GetCollisionSettings() override;
	bool HasCollisionMesh() override;
	bool GetCollisionMesh(FRuntimeMeshCollisionData& CollisionData) override;
	bool IsThreadSafe() override;

	void GeneratePointGrid();
	void InitializePointHeight(const int LinearIndex, const float HeightValue);
};
