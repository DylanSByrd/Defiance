#pragma once

#include <vector>
#include "Engine/Renderer/VertexComponent.hpp"


//-----------------------------------------------------------------------------------------------
class VertexDefinition
{
public:
   VertexDefinition() : m_components(), m_vertexSize(0) {}
   ~VertexDefinition() {}

   void AddComponent(const VertexComponent& component);
   int GetNumComponents() const { return m_components.size(); }
   int GetSizeOfVertex() const { return m_vertexSize; }
   const VertexComponent* GetComponentWithName(const std::string& name) const;

private:
   std::vector<VertexComponent> m_components;
   int m_vertexSize;
};