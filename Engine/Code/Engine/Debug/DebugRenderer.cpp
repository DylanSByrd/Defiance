#include "Engine/Debug/DebugRenderer.hpp"
#include "Engine/Debug/DebugCommands.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Time/Time.hpp"


//-----------------------------------------------------------------------------------------------
extern DebugRenderer* g_theDebugRenderer = nullptr;


//-----------------------------------------------------------------------------------------------
DebugRenderer::~DebugRenderer()
{
   for (auto commandIter = m_debugCommands.begin(); commandIter != m_debugCommands.end(); ++commandIter)
   {
      DebugCommand* command = *commandIter;
      delete command;
   }
}


//-----------------------------------------------------------------------------------------------
void DebugRenderer::Update()
{
   for (auto commandIter = m_debugCommands.begin(); commandIter != m_debugCommands.end();)
   {
      DebugCommand* command = *commandIter;
      float commandSecondsLeft = command->GetTimeToLive();

      // Draw infinitely
      if (commandSecondsLeft == -1.0f)
      {
         ++commandIter;
         continue;
      }

      commandSecondsLeft -= Time::GetDeltaSeconds();

      if (commandSecondsLeft < 0.f)
      {
         delete command;
         commandIter = m_debugCommands.erase(commandIter);
      }
      else
      {
         command->SetTimeToLive(commandSecondsLeft);
         ++commandIter;
      }
   }
}


//-----------------------------------------------------------------------------------------------
void DebugRenderer::Render() const
{
   for (const DebugCommand* const command : m_debugCommands)
   {
      command->Render();
   }
}


//-----------------------------------------------------------------------------------------------
void DebugRenderer::DrawPoint(float size, const Vector3f& position, const Rgba& lineColor, float timeToLive, DebugDepthMode depthMode)
{
   m_debugCommands.push_back(new DebugPointCommand(size, position, lineColor, timeToLive, depthMode));
}


//-----------------------------------------------------------------------------------------------
void DebugRenderer::DrawLine(const Vector3f& startPosition, const Vector3f& endPosition, const Rgba& lineColor, float timeToLive, DebugDepthMode depthMode)
{
   m_debugCommands.push_back(new DebugLineCommand(startPosition, endPosition, lineColor, timeToLive, depthMode));
}


//-----------------------------------------------------------------------------------------------
void DebugRenderer::DrawArrow(const Vector3f& startPosition, const Vector3f& endPosition, const Rgba& lineColor, float timeToLive, DebugDepthMode depthMode)
{
   m_debugCommands.push_back(new DebugArrowCommand(startPosition, endPosition, lineColor, timeToLive, depthMode));
}


//-----------------------------------------------------------------------------------------------
void DebugRenderer::DrawAABB3(const Vector3f& mins, const Vector3f& maxs, const Rgba& lineColor, const Rgba& fillColor, float timeToLive, DebugDepthMode depthMode)
{
   AABB3 bounds(mins, maxs);
   DrawAABB3(bounds, lineColor, fillColor, timeToLive, depthMode);
}


//-----------------------------------------------------------------------------------------------
void DebugRenderer::DrawAABB3(const AABB3& bounds, const Rgba& lineColor, const Rgba& fillColor, float timeToLive, DebugDepthMode depthMode)
{
   m_debugCommands.push_back(new DebugAABB3Command(bounds, lineColor, fillColor, timeToLive, depthMode));
}


//-----------------------------------------------------------------------------------------------
void DebugRenderer::DrawSphere(const Vector3f& centerPosition, float radius, const Rgba& lineColor, float timeToLive, DebugDepthMode depthMode)
{
   m_debugCommands.push_back(new DebugSphereCommand(centerPosition, radius, lineColor, timeToLive, depthMode));
}


//-----------------------------------------------------------------------------------------------
void DebugRenderer::DrawBasis(const Vector3f& startPosition, const Vector3f& forwardDir, const Vector3f& rightDir, const Vector3f& upDir, float lineLength, float timeToLive, DebugDepthMode depthMode)
{
   m_debugCommands.push_back(new DebugBasisCommand(startPosition, forwardDir, rightDir, upDir, lineLength, timeToLive, depthMode));
}