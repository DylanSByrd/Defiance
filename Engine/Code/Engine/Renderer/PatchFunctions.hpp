#pragma once

#include "Engine/Math/Vector3f.hpp"


//-----------------------------------------------------------------------------------------------
class MeshBuilder;


//-----------------------------------------------------------------------------------------------
struct plane_t
{
   Vector3f initialPosition;
   Vector3f right;
   Vector3f up;
};


//-----------------------------------------------------------------------------------------------
Vector3f PlaneFunc(void const *data, float x, float y);
Vector3f SinFunc(void const *data, float x, float y);


//-----------------------------------------------------------------------------------------------
void MeshBuildPlane(MeshBuilder *mb, Vector3f(*PatchFunc) (void const*, float x, float y),
   Vector3f initialPosition, Vector3f rightDir, Vector3f upDir,
   float xStart, float xEnd, size_t xSectionCount,
   float yStart, float yEnd, size_t ySectionCount);


//-----------------------------------------------------------------------------------------------
void MeshBuildPatch(MeshBuilder* mb, Vector3f(*PatchFunc) (const void*, float x, float y),
   void* user_data,
   float startx, float endx, size_t xSectionCount,
   float starty, float endy, size_t ySectionCount);