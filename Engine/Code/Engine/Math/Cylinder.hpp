#pragma once


//-----------------------------------------------------------------------------------------------
class Cylinder
{
public:
   Cylinder() {}
   Cylinder(float height, float radius) : m_height(height), m_radius(radius) {}
   Cylinder(const Cylinder& cylinder) : m_height(cylinder.m_height), m_radius(cylinder.m_radius) {}
   ~Cylinder() {}

   float m_height;
   float m_radius;
};