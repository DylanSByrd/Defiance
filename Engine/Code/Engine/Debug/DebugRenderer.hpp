#pragma once

#include <list>
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Debug/DebugCommands.hpp"


//-----------------------------------------------------------------------------------------------
class DebugRenderer;
extern DebugRenderer* g_theDebugRenderer;


//-----------------------------------------------------------------------------------------------
class Vector3f;
class AABB3;
class DebugCommand;


//-----------------------------------------------------------------------------------------------
class DebugRenderer
{
public:
   DebugRenderer() : m_debugCommands() {}
   ~DebugRenderer();

   void Update();
   void Render() const;
   void DrawPoint(float size, const Vector3f& position, const Rgba& lineColor = Rgba::WHITE, float timeToLive = -1.f, DebugDepthMode depthMode = DEPTH_ON);
   void DrawLine(const Vector3f& startPosition, const Vector3f& endPosition, const Rgba& lineColor = Rgba::WHITE, float timeToLive = -1.f, DebugDepthMode depthMode = DEPTH_ON);
   void DrawArrow(const Vector3f& startPosition, const Vector3f& endPosition, const Rgba& lineColor = Rgba::WHITE, float timeToLive = -1.f, DebugDepthMode depthMode = DEPTH_ON);
   void DrawAABB3(const Vector3f& mins, const Vector3f& maxs, const Rgba& lineColor = Rgba::WHITE, const Rgba& fillColor = Rgba::WHITE, float timeToLive = -1.f, DebugDepthMode depthMode = DEPTH_ON);
   void DrawAABB3(const AABB3& bounds, const Rgba& lineColor = Rgba::WHITE, const Rgba& fillColor = Rgba::WHITE, float timeToLive = -1.f, DebugDepthMode depthMode = DEPTH_ON);
   void DrawSphere(const Vector3f& centerPosition, float radius, const Rgba& lineColor = Rgba::WHITE, float timeToLive = -1.f, DebugDepthMode depthMode = DEPTH_ON);
   void DrawBasis(const Vector3f& startPosition, const Vector3f& forwardDir, const Vector3f& rightDir, const Vector3f& upDir, float lineLength = 1.f, float timeToLive = -1.f, DebugDepthMode depthMode = DUAL_DEPTH);

private:
   std::list<DebugCommand*> m_debugCommands;
};