#pragma once

#include "Engine/Renderer/PatchFunctions.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"


Vector3f PlaneFunc(void const *data, float x, float y)
{
   plane_t* plane = (plane_t*)data;
   return plane->initialPosition
      + plane->right * x
      + plane->up * y;
}


Vector3f SinFunc(void const *data, float x, float y)
{
   plane_t* plane = (plane_t*)data;
   Vector3f heightComponent(0.f, sin(x) + sin(y), 0.f);
   return plane->initialPosition
      + plane->right * x
      + plane->up * y
      + heightComponent;
}

// #TODO - clean this mess up
void MeshBuildPlane(MeshBuilder *mb, Vector3f(*PatchFunc) (void const*, float x, float y),
   Vector3f initialPosition, Vector3f rightDir, Vector3f upDir,
   float xStart, float xEnd, size_t xSectionCount,
   float yStart, float yEnd, size_t ySectionCount)
{
   plane_t plane;
   plane.initialPosition = initialPosition;
   plane.right = rightDir;
   plane.up = upDir;

   MeshBuildPatch(mb, PatchFunc, &plane, xStart, xEnd, xSectionCount, yStart, yEnd, ySectionCount);
}

void MeshBuildPatch(MeshBuilder* mb, Vector3f(*PatchFunc) (void const*, float x, float y),
   void* user_data,
   float xStart, float xEnd, size_t xSectionCount,
   float yStart, float yEnd, size_t ySectionCount)
{
   ASSERT_OR_DIE(xSectionCount > 0, "No xCount for patch!");
   ASSERT_OR_DIE(ySectionCount > 0, "No yCount for patch!");

   mb->BeginDrawInstruction();
   uint32_t xVertexCount = xSectionCount + 1;
   uint32_t yVertexCount = ySectionCount + 1;

   const float xRange = xEnd - xStart;
   const float yRange = yEnd - yStart;
   const float xStep = xRange / (float)xSectionCount;
   const float yStep = yRange / (float)ySectionCount;

   const float uStep = 1.f / (float)xSectionCount;
   const float vStep = 1.f / (float)ySectionCount;

   const float delta = .001f;

   float x, y;
   float u, v;

   y = 0.0f;
   v = 0.0f;

   uint32_t startVertexIndex = mb->GetVertexIndex();

   // Generates all the vertices
   for (uint32_t iy = 0; iy < yVertexCount; ++iy)
   {
      x = 0.f;
      u = 0.f;
      for (uint32_t ix = 0; ix < xVertexCount; ++ix)
      {
         mb->SetCurrentVertexUV0(TexCoords(u, v));

         Vector3f tangent = PatchFunc(user_data, x + delta, y) - PatchFunc(user_data, x - delta, y);
         tangent.Normalize();
         Vector3f bitangent = PatchFunc(user_data, x, y + delta) - PatchFunc(user_data, x, y - delta);
         bitangent.Normalize();
         Vector3f normal = bitangent.CrossProduct(tangent);
         bitangent = tangent.CrossProduct(normal);
         mb->SetCurrentVertexTBN(tangent, bitangent, normal);

         Vector3f position = PatchFunc(user_data, x, y);
         mb->SetCurrentVertexPosition(position);
         mb->AddCurrentVertexToMesh();

         x += xStep;
         u += uStep;
      }
      y += yStep;
      v += vStep;
   }

   // Add indices
   for (uint32_t iy = 0; iy < ySectionCount; ++iy)
   {
      for (uint32_t ix = 0; ix < xSectionCount; ++ix)
      {
         uint32_t blIndex = startVertexIndex
            + (iy * xVertexCount) + ix;

         uint32_t brIndex = blIndex + 1;
         uint32_t tlIndex = blIndex + xVertexCount;
         uint32_t trIndex = tlIndex + 1;
         
         mb->AddQuadIndices(blIndex, brIndex, trIndex, tlIndex);
      }
   }

   mb->EndDrawInstruction();
}