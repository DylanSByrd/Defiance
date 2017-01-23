#include "Engine/Renderer/VertexDefinition.hpp"


//-----------------------------------------------------------------------------------------------
void VertexDefinition::AddComponent(const VertexComponent& component)
{
   m_components.push_back(component);
   
   int elementCount = component.count;
   const ElementType& elementType = component.type;
   int elementSize = 0;

   // add cases as necessary
   switch (elementType)
   {
   case INT_ELEMENT:
   {
      elementSize = sizeof(int);
      break;
   }
   
   case FLOAT_ELEMENT:
   {
      elementSize = sizeof(float);
      break;
   }
         
   case UNSIGNED_BYTE_ELEMENT:
   {
      elementSize = sizeof(unsigned char);
      break;
   }
   }

   m_vertexSize += elementSize * elementCount;
}


//-----------------------------------------------------------------------------------------------
const VertexComponent* VertexDefinition::GetComponentWithName(const std::string& name) const
{
   for (const VertexComponent& component : m_components)
   {
      if (!name.compare(component.name))
      {
         return &component;
      }
   }

   return nullptr;
}