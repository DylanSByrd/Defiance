#pragma once

#include "Engine/Math/Vector3f.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Math/AABB3.hpp"


//-----------------------------------------------------------------------------------------------
enum DebugDepthMode
{
   DEPTH_ON,
   DEPTH_OFF,
   DUAL_DEPTH,
   NUM_DEPTH_MODES
};


//-----------------------------------------------------------------------------------------------
class DebugCommand
{
public:
   DebugCommand() : m_timeToLive(0.f), m_lineColor(Rgba::WHITE), m_depthMode(DEPTH_ON) {}
   DebugCommand(float lifeInSeconds, const Rgba& lineColor, DebugDepthMode depthMode) : m_timeToLive(lifeInSeconds), m_lineColor(lineColor), m_depthMode(depthMode) {}
   virtual ~DebugCommand() {}

   virtual void Render() const = 0;

   float GetTimeToLive() const { return m_timeToLive; }
   void SetTimeToLive(float secondsToLive) { m_timeToLive = secondsToLive; }

protected:
   float m_timeToLive;
   Rgba m_lineColor;
   DebugDepthMode m_depthMode;
};


//-----------------------------------------------------------------------------------------------
class DebugPointCommand : public DebugCommand
{
public:
   DebugPointCommand(float size, const Vector3f& position, const Rgba& lineColor, float timeToLive, DebugDepthMode depthMode);
   ~DebugPointCommand() {}

   virtual void Render() const;

private:
   float m_size;
   Vector3f m_position;
};


//-----------------------------------------------------------------------------------------------
class DebugLineCommand : public DebugCommand
{
public:
   DebugLineCommand(const Vector3f& startPosition, const Vector3f& endPosition, const Rgba& lineColor, float timeToLive, DebugDepthMode depthMode);
   ~DebugLineCommand() {}

   virtual void Render() const;

private:
   Vector3f m_startPosition;
   Vector3f m_endPosition;
};


//-----------------------------------------------------------------------------------------------
class DebugArrowCommand : public DebugCommand
{
public:
   DebugArrowCommand(const Vector3f& startPosition, const Vector3f& endPosition, const Rgba& lineColor, float timeToLive, DebugDepthMode depthMode);
   ~DebugArrowCommand() {}

   virtual void Render() const;

private:
   Vector3f m_startPosition;
   Vector3f m_endPosition;
};


//-----------------------------------------------------------------------------------------------
class DebugAABB3Command : public DebugCommand
{
public:
   DebugAABB3Command(const AABB3& bounds, const Rgba& lineColor, const Rgba& fillColor, float timeToLive, DebugDepthMode depthMode);
   ~DebugAABB3Command() {}

   virtual void Render() const;

private:
   AABB3 m_bounds;
   Rgba m_fillColor;
};


//-----------------------------------------------------------------------------------------------
class DebugSphereCommand : public DebugCommand
{
public:
   DebugSphereCommand(const Vector3f& centerPosition, float radius, const Rgba& lineColor = Rgba::WHITE, float timeToLive = -1.f, DebugDepthMode depthMode = DEPTH_ON);
   ~DebugSphereCommand() {}

   virtual void Render() const;

private:
   Vector3f m_centerPos;
   float m_radius;
};


//-----------------------------------------------------------------------------------------------
class DebugBasisCommand : public DebugCommand
{
public:
   DebugBasisCommand(const Vector3f& startPosition, const Vector3f& forwardDir, const Vector3f& rightDir, const Vector3f& upDir, float lineLength, float timeToLive, DebugDepthMode depthMode);
   DebugBasisCommand() {}

   virtual void Render() const;

private:
   Vector3f m_startPosition;
   Vector3f m_forwardDir;
   Vector3f m_rightDir;
   Vector3f m_upDir;
   float m_lineLength;
};