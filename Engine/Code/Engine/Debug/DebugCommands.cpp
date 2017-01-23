#include <vector>
#include "Engine/Debug/DebugCommands.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
DebugPointCommand::DebugPointCommand(float size, const Vector3f& position, const Rgba& lineColor, float timeToLive, DebugDepthMode depthMode)
   : DebugCommand(timeToLive, lineColor, depthMode)
   , m_size(size)
   , m_position(position)
{
}


//-----------------------------------------------------------------------------------------------
void DebugPointCommand::Render() const
{
   switch (m_depthMode)
   {
   case DEPTH_ON:
   case DUAL_DEPTH:
   {
      g_theRenderer->EnableDepthTesting();
      break;
   }
   case DEPTH_OFF:
   {
      g_theRenderer->DisableDepthTesting();
      break;
   }
   }

   float halfSize = m_size / 2.f;
   g_theRenderer->DrawLine(Vector3f(m_position.x - halfSize, m_position.y, m_position.z), Vector3f(m_position.x + halfSize, m_position.y, m_position.z), m_lineColor, 3.f);
   g_theRenderer->DrawLine(Vector3f(m_position.x, m_position.y - halfSize, m_position.z), Vector3f(m_position.x, m_position.y + halfSize, m_position.z), m_lineColor, 3.f);
   g_theRenderer->DrawLine(Vector3f(m_position.x, m_position.y, m_position.z - halfSize), Vector3f(m_position.x, m_position.y, m_position.z + halfSize), m_lineColor, 3.f);

   if (m_depthMode == DUAL_DEPTH)
   {
      g_theRenderer->DisableDepthTesting();
      Rgba faintColor = m_lineColor;
      faintColor.a /= 3;

      g_theRenderer->DrawLine(Vector3f(m_position.x - halfSize, m_position.y, m_position.z), Vector3f(m_position.x + halfSize, m_position.y, m_position.z), faintColor, 1.f);
      g_theRenderer->DrawLine(Vector3f(m_position.x, m_position.y - halfSize, m_position.z), Vector3f(m_position.x, m_position.y + halfSize, m_position.z), faintColor, 1.f);
      g_theRenderer->DrawLine(Vector3f(m_position.x, m_position.y, m_position.z - halfSize), Vector3f(m_position.x, m_position.y, m_position.z + halfSize), faintColor, 1.f);
   }
}


//-----------------------------------------------------------------------------------------------
DebugLineCommand::DebugLineCommand(const Vector3f& startPosition, const Vector3f& endPosition, const Rgba& lineColor, float timeToLive, DebugDepthMode depthMode)
   : DebugCommand(timeToLive, lineColor, depthMode)
   , m_startPosition(startPosition)
   , m_endPosition(endPosition)
{
}


//-----------------------------------------------------------------------------------------------
void DebugLineCommand::Render() const
{
   switch (m_depthMode)
   {
   case DEPTH_ON:
   case DUAL_DEPTH:
   {
      g_theRenderer->EnableDepthTesting();
      break;
   }
   case DEPTH_OFF:
   {
      g_theRenderer->DisableDepthTesting();
      break;
   }
   }

   g_theRenderer->DrawLine(m_startPosition, m_endPosition, m_lineColor, 3.f);

   if (m_depthMode == DUAL_DEPTH)
   {
      g_theRenderer->DisableDepthTesting();
      Rgba faintColor = m_lineColor;
      faintColor.a /= 3;

      g_theRenderer->DrawLine(m_startPosition, m_endPosition, faintColor, 1.f);
   }
}


//-----------------------------------------------------------------------------------------------
DebugArrowCommand::DebugArrowCommand(const Vector3f& startPosition, const Vector3f& endPosition, const Rgba& lineColor, float timeToLive, DebugDepthMode depthMode)
   : DebugCommand(timeToLive, lineColor, depthMode)
   , m_startPosition(startPosition)
   , m_endPosition(endPosition)
{
}


//-----------------------------------------------------------------------------------------------
void DebugArrowCommand::Render() const
{
   switch (m_depthMode)
   {
   case DEPTH_ON:
   case DUAL_DEPTH:
   {
      g_theRenderer->EnableDepthTesting();
      break;
   }
   case DEPTH_OFF:
   {
      g_theRenderer->DisableDepthTesting();
      break;
   }
   }

   g_theRenderer->DrawLine(m_startPosition, m_endPosition, m_lineColor, 3.f);

   // calculate end positions of arrow tip
   // Get reverse direction and worldUp vector
   Vector3f tipDir(m_startPosition - m_endPosition);
   tipDir.Normalize();
   Vector3f worldUp(0.f, 1.f, 0.f); // Doesn't have to be game's worldUp

   // Cross tip dir and world up to get perpendicular lines
   Vector3f perpToTipDir1 = tipDir.CrossProduct(worldUp);
   Vector3f tip1 = tipDir + perpToTipDir1;
   Vector3f tip2 = tipDir - perpToTipDir1;

   Vector3f perpToTipDir2 = perpToTipDir1.CrossProduct(tipDir);
   Vector3f tip3 = tipDir + perpToTipDir2;
   Vector3f tip4 = tipDir - perpToTipDir2;

   // Scale tips by arrow length
   float scale = (m_endPosition - m_startPosition).GetLength() / 15.f;
   tip1 *= scale;
   tip2 *= scale;
   tip3 *= scale;
   tip4 *= scale;

   g_theRenderer->DrawLine(m_endPosition, m_endPosition + tip1, m_lineColor, 3.f);
   g_theRenderer->DrawLine(m_endPosition, m_endPosition + tip2, m_lineColor, 3.f);
   g_theRenderer->DrawLine(m_endPosition + tip1, m_endPosition + tip2, m_lineColor, 3.f);
   g_theRenderer->DrawLine(m_endPosition, m_endPosition + tip3, m_lineColor, 3.f);
   g_theRenderer->DrawLine(m_endPosition, m_endPosition + tip4, m_lineColor, 3.f);
   g_theRenderer->DrawLine(m_endPosition + tip3, m_endPosition + tip4, m_lineColor, 3.f);
   g_theRenderer->DrawLine(m_endPosition + tip1, m_endPosition + tip3, m_lineColor, 3.f);
   g_theRenderer->DrawLine(m_endPosition + tip1, m_endPosition + tip4, m_lineColor, 3.f);
   g_theRenderer->DrawLine(m_endPosition + tip2, m_endPosition + tip3, m_lineColor, 3.f);
   g_theRenderer->DrawLine(m_endPosition + tip2, m_endPosition + tip4, m_lineColor, 3.f);


   if (m_depthMode == DUAL_DEPTH)
   {
      g_theRenderer->DisableDepthTesting();
      Rgba faintColor = m_lineColor;
      faintColor.a /= 3;

      g_theRenderer->DrawLine(m_startPosition, m_endPosition, faintColor, 1.f);
      g_theRenderer->DrawLine(m_endPosition, m_endPosition + tip1, faintColor, 1.f);
      g_theRenderer->DrawLine(m_endPosition, m_endPosition + tip2, faintColor, 1.f);
      g_theRenderer->DrawLine(m_endPosition + tip1, m_endPosition + tip2, faintColor, 1.f);
      g_theRenderer->DrawLine(m_endPosition, m_endPosition + tip3, faintColor, 1.f);
      g_theRenderer->DrawLine(m_endPosition, m_endPosition + tip4, faintColor, 1.f);
      g_theRenderer->DrawLine(m_endPosition + tip3, m_endPosition + tip4, faintColor, 1.f);
      g_theRenderer->DrawLine(m_endPosition + tip1, m_endPosition + tip3, faintColor, 1.f);
      g_theRenderer->DrawLine(m_endPosition + tip1, m_endPosition + tip4, faintColor, 1.f);
      g_theRenderer->DrawLine(m_endPosition + tip2, m_endPosition + tip3, faintColor, 1.f);
      g_theRenderer->DrawLine(m_endPosition + tip2, m_endPosition + tip4, faintColor, 1.f);
   }
}


//-----------------------------------------------------------------------------------------------
DebugAABB3Command::DebugAABB3Command(const AABB3& bounds, const Rgba& lineColor, const Rgba& fillColor, float timeToLive, DebugDepthMode depthMode)
   : DebugCommand(timeToLive, lineColor, depthMode)
   , m_bounds(bounds)
   , m_fillColor(fillColor)
{
}


//-----------------------------------------------------------------------------------------------
void DebugAABB3Command::Render() const
{
   switch (m_depthMode)
   {
   case DEPTH_ON:
   case DUAL_DEPTH:
   {
      g_theRenderer->EnableDepthTesting();
      break;
   }
   case DEPTH_OFF:
   {
      g_theRenderer->DisableDepthTesting();
      break;
   }
   }

   g_theRenderer->EnableBackFaceCulling();
   g_theRenderer->EnableWireFrameFilling();

   // Although faces are labeled "top", "bot" etc., these are basis-independent
   // TODO: Move logic into a DrawAABB3 in renderer, with option for hollow
   // Top Face  
   g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                               , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                               , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                               , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                               , m_lineColor, 3.f);
   g_theRenderer->Draw3DQuad(Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                           , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                           , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                           , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                           , m_fillColor);

   // Bot Face
   g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                               , Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                               , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                               , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                               , m_lineColor, 3.f);
   g_theRenderer->Draw3DQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                           , Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                           , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                           , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                           , m_fillColor);

   // Left Face
   g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                               , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                               , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                               , Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                               , m_lineColor, 3.f);
   g_theRenderer->Draw3DQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                           , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                           , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                           , Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                           , m_fillColor);

   // Right Face
   g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                               , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                               , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                               , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                               , m_lineColor, 3.f);
   g_theRenderer->Draw3DQuad(Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                           , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                           , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                           , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                           , m_fillColor);

   // Front Face
   g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                               , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                               , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                               , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                               , m_lineColor, 3.f);
   g_theRenderer->Draw3DQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                           , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                           , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                           , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                           , m_fillColor);

   // Back Face
   g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                               , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                               , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                               , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                               , m_lineColor, 3.f);
   g_theRenderer->Draw3DQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                           , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                           , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                           , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                           , m_fillColor);

   if (m_depthMode == DUAL_DEPTH)
   {
      g_theRenderer->DisableDepthTesting();
      Rgba faintColor = m_lineColor;
      faintColor.a /= 3;
      Rgba faintFillColor = m_fillColor;
      faintFillColor.a /= 3;

      // Top Face  
      g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                                  , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                                  , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                                  , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                                  , faintColor, 1.f);
      g_theRenderer->Draw3DQuad(Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                              , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                              , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                              , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                              , m_fillColor);

      // Bot Face
      g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                                  , Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                                  , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                                  , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                                  , faintColor, 1.f);
      g_theRenderer->Draw3DQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                              , Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                              , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                              , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                              , m_fillColor);

      // Left Face
      g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                                  , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                                  , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                                  , Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                                  , faintColor, 1.f);
      g_theRenderer->Draw3DQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                              , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                              , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                              , Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                              , m_fillColor);

      // Right Face
      g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                                  , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                                  , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                                  , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                                  , faintColor, 1.f);
      g_theRenderer->Draw3DQuad(Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                              , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                              , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                              , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                              , m_fillColor);

      // Front Face
      g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                                  , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                                  , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                                  , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                                  , faintColor, 1.f);
      g_theRenderer->Draw3DQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.maxs.z)
                              , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.maxs.z)
                              , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.maxs.z)
                              , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.maxs.z)
                              , m_fillColor);

      // Back Face
      g_theRenderer->DrawHollowQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                                  , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                                  , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                                  , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                                  , faintColor, 1.f);
      g_theRenderer->Draw3DQuad(Vector3f(m_bounds.mins.x, m_bounds.mins.y, m_bounds.mins.z)
                              , Vector3f(m_bounds.maxs.x, m_bounds.mins.y, m_bounds.mins.z)
                              , Vector3f(m_bounds.maxs.x, m_bounds.maxs.y, m_bounds.mins.z)
                              , Vector3f(m_bounds.mins.x, m_bounds.maxs.y, m_bounds.mins.z)
                              , m_fillColor);
   }
}


//-----------------------------------------------------------------------------------------------
DebugSphereCommand::DebugSphereCommand(const Vector3f& centerPosition, float radius, const Rgba& lineColor, float timeToLive, DebugDepthMode depthMode)
   : DebugCommand(timeToLive, lineColor, depthMode)
   , m_centerPos(centerPosition)
   , m_radius(radius)
{}


//-----------------------------------------------------------------------------------------------
void DebugSphereCommand::Render() const
{
   switch (m_depthMode)
   {
   case DEPTH_ON:
   case DUAL_DEPTH:
   {
      g_theRenderer->EnableDepthTesting();
      break;
   }
   case DEPTH_OFF:
   {
      g_theRenderer->DisableDepthTesting();
      break;
   }
   }

   std::vector<Vertex_PCT> verts;
   const float diameter = m_radius * 2.f;
   const float stackAngleRadians = TAU / (float)30;
   const float sliceAngleRadians = TAU / (float)30;

   for (int stackIndex = 0; stackIndex < 30; ++stackIndex)
   {
      float currentHeight = m_radius - ((diameter / 30) * stackIndex);

      float temp = RangeMap((float)stackIndex, 0.f, (float)30 - 1.f, -1.f, 1.f);
      float currentWidth = sqrt(1.f - (temp * temp)) * m_radius;

      for (int sliceIndex = 0; sliceIndex < 30; ++sliceIndex)
      {
         float thisSliceAngle = sliceAngleRadians * sliceIndex;
         float x = currentWidth * cos(thisSliceAngle);
         float z = currentWidth * sin(thisSliceAngle);
         Vector3f newPoint(x, currentHeight, z);
         verts.push_back(Vertex_PCT(m_centerPos + newPoint, m_lineColor, TexCoords(x, (float)stackIndex / (float)(30 - 1))));
      }
   }
   g_theRenderer->DrawVertexArray_PCT(&verts[0], verts.size(), PRIMITIVE_LINE_LOOP);



   // TODO: FIX THIS IN THE RENDERER
//    const float radiansPerSide = TAU / (float)50;
//    int numVertices = (int)((360.f / radiansPerSide) + 1);
// 
//    std::vector<Vertex_PCT> vertexesX;
//    std::vector<Vertex_PCT> vertexesY;
//    std::vector<Vertex_PCT> vertexesZ;
//    vertexesX.reserve(numVertices);
//    vertexesY.reserve(numVertices);
//    vertexesZ.reserve(numVertices);
// 
//    // build circles
//    for (float radians = 0.f; radians < TAU; radians += radiansPerSide)
//    {
//       float x = m_radius * cos(radians);
//       float y = m_radius * sin(radians);
// 
//       vertexesX.push_back(Vertex_PCT(m_centerPos + Vector3f(0.f, x, y), m_lineColor));
//       vertexesY.push_back(Vertex_PCT(m_centerPos + Vector3f(x, 0.f, y), m_lineColor));
//       vertexesZ.push_back(Vertex_PCT(m_centerPos + Vector3f(x, y, 0.f), m_lineColor));
//    }
//    g_theRenderer->DrawVertexArray_PCT(&vertexesX[0], vertexesX.size(), TheRenderer::PRIMITIVE_LINE_LOOP);
//    g_theRenderer->DrawVertexArray_PCT(&vertexesY[0], vertexesY.size(), TheRenderer::PRIMITIVE_LINE_LOOP);
//    g_theRenderer->DrawVertexArray_PCT(&vertexesZ[0], vertexesZ.size(), TheRenderer::PRIMITIVE_LINE_LOOP);

   if (m_depthMode == DUAL_DEPTH)
   {
      g_theRenderer->DisableDepthTesting();
      Rgba faintColor = m_lineColor;
      faintColor.a /= 3;

      for (Vertex_PCT& vert : verts)
      {
         vert.color = faintColor;
      }

//       for (Vertex_PCT& vert : vertexesX)
//       {
//          vert.m_color = faintColor;
//       }
// 
//       for (Vertex_PCT& vert : vertexesY)
//       {
//          vert.m_color = faintColor;
//       }
// 
//       for (Vertex_PCT& vert : vertexesZ)
//       {
//          vert.m_color = faintColor;
//       }
// 
//       g_theRenderer->DrawVertexArray_PCT(&vertexesX[0], vertexesX.size(), TheRenderer::PRIMITIVE_LINE_LOOP);
//       g_theRenderer->DrawVertexArray_PCT(&vertexesY[0], vertexesY.size(), TheRenderer::PRIMITIVE_LINE_LOOP);
//       g_theRenderer->DrawVertexArray_PCT(&vertexesZ[0], vertexesZ.size(), TheRenderer::PRIMITIVE_LINE_LOOP);
   }
}


//-----------------------------------------------------------------------------------------------
DebugBasisCommand::DebugBasisCommand(const Vector3f& startPosition, const Vector3f& forwardDir, const Vector3f& rightDir, const Vector3f& upDir, float lineLength, float timeToLive, DebugDepthMode depthMode)
   : DebugCommand(timeToLive, Rgba::WHITE, depthMode)
   , m_startPosition(startPosition)
   , m_forwardDir(forwardDir)
   , m_rightDir(rightDir)
   , m_upDir(upDir)
   , m_lineLength(lineLength)
{
}


//-----------------------------------------------------------------------------------------------
void DebugBasisCommand::Render() const
{
   switch (m_depthMode)
   {
   case DEPTH_ON:
   case DUAL_DEPTH:
   {
      g_theRenderer->EnableDepthTesting();
      break;
   }
   case DEPTH_OFF:
   {
      g_theRenderer->DisableDepthTesting();
      break;
   }
   }

   const Rgba rightColor = Rgba::RED;
   const Rgba upColor = Rgba::GREEN;
   const Rgba forwardColor = Rgba::BLUE;

   Vector3f rightEndPos = m_startPosition + (m_rightDir * m_lineLength);
   Vector3f forwardEndPos = m_startPosition + (m_forwardDir * m_lineLength);
   Vector3f upEndPos = m_startPosition + (m_upDir * m_lineLength);

   g_theRenderer->DrawLine(m_startPosition, rightEndPos, rightColor, 3.f);
   g_theRenderer->DrawLine(m_startPosition, forwardEndPos, forwardColor, 3.f);
   g_theRenderer->DrawLine(m_startPosition, upEndPos, upColor, 3.f);

   if (m_depthMode == DUAL_DEPTH)
   {
      g_theRenderer->DisableDepthTesting();
      
      Rgba faintRightColor = rightColor;
      faintRightColor.a /= 3;
      Rgba faintUpColor = upColor;
      faintUpColor.a /= 3;
      Rgba faintForwardColor = forwardColor;
      faintForwardColor.a /= 3;

      g_theRenderer->DrawLine(m_startPosition, rightEndPos, faintRightColor, 1.f);
      g_theRenderer->DrawLine(m_startPosition, forwardEndPos, faintForwardColor, 1.f);
      g_theRenderer->DrawLine(m_startPosition, upEndPos, faintUpColor, 1.f);
   }
}