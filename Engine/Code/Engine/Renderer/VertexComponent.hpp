#pragma once

#include <string>
#include "Engine/Renderer/RendererCommon.hpp"


//-----------------------------------------------------------------------------------------------
struct VertexComponent
{
   enum ComponentHint
   {
      POSITION,
      COLOR,
      UV,
      TANGENT,
      BITANGENT,
      NORMAL,
      BONE_INDICES,
      BONE_WEIGHTS,
      NUM_COMPONENT_TYPES,
   };

   VertexComponent(const std::string& newName, ComponentHint newHint, int newCount, ElementType newType, bool doNormalize, GLvoid* newOffset);

   std::string name;
   ComponentHint hint;
   int count;
   ElementType type;
   bool isNormalized;
   GLvoid* offset;
};


//-----------------------------------------------------------------------------------------------
inline VertexComponent::VertexComponent(const std::string& newName, ComponentHint newHint, int newCount, ElementType newType, bool doNormalize, GLvoid* newOffset)
   : name(newName)
   , hint(newHint)
   , count(newCount)
   , type(newType)
   , isNormalized(doNormalize)
   , offset(newOffset)
{
}