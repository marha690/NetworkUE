// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainMeshProvider.h"

UMaterialInterface* UTerrainMeshProvider::GetBoxMaterial() const
{
	FScopeLock Lock(&PropertySyncRoot);
	return Material;
}

void UTerrainMeshProvider::SetBoxMaterial(UMaterialInterface* InMaterial)
{
	FScopeLock Lock(&PropertySyncRoot);
	Material = InMaterial;
	SetupMaterialSlot(0, FName("Cube Base"), Material);
}

bool UTerrainMeshProvider::UpdateHeight(const int X, const int Y, const float HeightModification)
{
	return UpdateHeight(LinearIndex(X,Y), HeightModification);
}

bool UTerrainMeshProvider::UpdateHeight(const int Index, const float HeightModification)
{
	float NewHeight = CurrentHeight[Index] + HeightModification;
	float MaxHeight = DefaultHeight[Index] + AllowedHeightDifference;
	float MinHeight = DefaultHeight[Index] - AllowedHeightDifference;

	// Check limits of the terrain
	bool IsMagnitudeToBig = false;
	if (NewHeight > MaxHeight) {
		CurrentHeight[Index] = MaxHeight;
		return false;
	}
	if (NewHeight < MinHeight) {
		CurrentHeight[Index] = MinHeight;
		return false;
	}

	CurrentHeight[Index] = NewHeight;
	return true;
}

FVector UTerrainMeshProvider::GetTerrainPointPosition(const int Index)
{
	return FVector(PointPosition[Index].X, PointPosition[Index].Y, CurrentHeight[Index]);
}

inline int UTerrainMeshProvider::LinearIndex(const int X, const int Y)
{
	return (X + Y * PointsPerLine);
}

inline bool UTerrainMeshProvider::CubicIndex(const int LinearIndex, int &X, int &Y)
{
	X = LinearIndex % PointsPerLine;
	Y = LinearIndex / PointsPerLine;

	if(X > 0 && X < PointsPerLine && 
	   Y > 0 && Y < PointsPerLine)
		return false; // out of bounds for this grid.

	return true; // index is inside this grid.
}

void UTerrainMeshProvider::Initialize()
{
	GeneratePointGrid();

	FRuntimeMeshLODProperties LODProperties;
	LODProperties.ScreenSize = 0.0f;

	ConfigureLODs({ LODProperties });

	SetupMaterialSlot(0, FName("Cube Base"), GetBoxMaterial());

	FRuntimeMeshSectionProperties Properties;
	Properties.bCastsShadow = true;
	Properties.bIsVisible = true;
	Properties.MaterialSlot = 0;
	Properties.UpdateFrequency = ERuntimeMeshUpdateFrequency::Infrequent;
	CreateSection(0, 0, Properties);

	MarkCollisionDirty();
}

void UTerrainMeshProvider::SetTerrainInfo(float Size, int SubdivisionLevel)
{
	TileSize = Size;
	Subdivision = SubdivisionLevel;
	PointsPerLine = Subdivision + 1;
	NumberOfPoints = PointsPerLine * PointsPerLine;
}

FBoxSphereBounds UTerrainMeshProvider::GetBounds()
{
	float HalfTile = TileSize * 0.5f;
	FVector BoxRadius{ -HalfTile, HalfTile, -3000 };
	return FBoxSphereBounds(FBox(-BoxRadius, BoxRadius));
}

bool UTerrainMeshProvider::GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData)
{
	check(SectionId == 0 && LODIndex == 0);

	FVector TangentX, TangentY, TangentZ;
	auto AddVertex = [&](const FVector& InPosition, const FVector& InTangentX, const FVector& InTangentZ, const FVector2D& InTexCoord)
	{
		MeshData.Positions.Add(InPosition);
		MeshData.Tangents.Add(InTangentZ, InTangentX);
		MeshData.Colors.Add(FColor::White);
		MeshData.TexCoords.Add(InTexCoord);
	};


	TangentZ = FVector(0.0f, 0.0f, 1.0f);
	TangentX = FVector(0.0f, -1.0f, 0.0f);
	int c = 0;
	for (size_t i = 0; i < Subdivision; i++)
		for (size_t j = 0; j < Subdivision; j++)
		{
			int LIndex = LinearIndex(i, j);
			int X, Y;
			CubicIndex(LIndex, X, Y);

			// Vertex data
			AddVertex(GetTerrainPointPosition(LinearIndex(X + 0, Y + 0)), TangentX, TangentZ, FVector2D(0.0f, 0.0f));
			AddVertex(GetTerrainPointPosition(LinearIndex(X + 1, Y + 0)), TangentX, TangentZ, FVector2D(0.0f, 1.0f));
			AddVertex(GetTerrainPointPosition(LinearIndex(X + 0, Y + 1)), TangentX, TangentZ, FVector2D(1.0f, 0.0f));
			AddVertex(GetTerrainPointPosition(LinearIndex(X + 1, Y + 1)), TangentX, TangentZ, FVector2D(1.0f, 1.0f));

			// Triangles
			MeshData.Triangles.AddTriangle(c + 0, c + 2, c + 3);
			MeshData.Triangles.AddTriangle(c + 0, c + 3, c + 1);
			c += 4;
		}

	return true;
}

FRuntimeMeshCollisionSettings UTerrainMeshProvider::GetCollisionSettings()
{
	FRuntimeMeshCollisionSettings Settings;
	Settings.bUseAsyncCooking = true;
	Settings.bUseComplexAsSimple = true;
	return Settings;
}

bool UTerrainMeshProvider::HasCollisionMesh()
{
	return true;
}

bool UTerrainMeshProvider::GetCollisionMesh(FRuntimeMeshCollisionData& CollisionData)
{
	// Add the single collision section
	CollisionData.CollisionSources.Emplace(0, 5, this, 0, ERuntimeMeshCollisionFaceSourceType::Collision);

	FRuntimeMeshCollisionVertexStream& CollisionVertices = CollisionData.Vertices;
	FRuntimeMeshCollisionTriangleStream& CollisionTriangles = CollisionData.Triangles;

	// Generate verts
	for (size_t i = 0; i < NumberOfPoints; i++)
	{
		CollisionVertices.Add(GetTerrainPointPosition(i));
	}

	// Triangles
	for (size_t i = 0; i < Subdivision; i++)
		for (size_t j = 0; j < Subdivision; j++)
		{
			int LIndex = LinearIndex(i, j);
			int X, Y;
			CubicIndex(LIndex, X, Y);

			CollisionTriangles.Add(LinearIndex(X + 0, Y + 0), LinearIndex(X + 0, Y + 1), LinearIndex(X + 1, Y + 0));
			CollisionTriangles.Add(LinearIndex(X + 1, Y + 0), LinearIndex(X + 0, Y + 1), LinearIndex(X + 1, Y + 1));
		}

	return true;
}

bool UTerrainMeshProvider::IsThreadSafe()
{
	return true;
}

void UTerrainMeshProvider::GeneratePointGrid()
{
	PointPosition.Reserve(NumberOfPoints);
	CurrentHeight.Reserve(NumberOfPoints);
	DefaultHeight.Reserve(NumberOfPoints);

	float HalfTileSize = TileSize * 0.5;
	float TriangleSize = TileSize / static_cast<float>(Subdivision);
	for (size_t i = 0; i < NumberOfPoints; i++)
	{
		int X, Y;
		CubicIndex(i, X, Y);

		float XPosition = -HalfTileSize + X * TriangleSize;
		float YPosition = -HalfTileSize + Y * TriangleSize;
		float Height = 200.f;

		PointPosition.Add(FVector2D(XPosition, YPosition));
		InitializePointHeight(i, 0.0f);
	}
}

void UTerrainMeshProvider::InitializePointHeight(const int LinearIndex, const float HeightValue)
{
	DefaultHeight.Add(HeightValue);
	CurrentHeight.Add(HeightValue);
}
