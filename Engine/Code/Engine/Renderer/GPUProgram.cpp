#include "Engine/Renderer/GPUProgram.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"




//-----------------------------------------------------------------------------------------------
void GPUProgram::AddAttribute(const std::string& name, const GPUProgramAttribute& attribute)
{
   m_attributes[name] = attribute;
}


//-----------------------------------------------------------------------------------------------
const GPUProgramAttribute& GPUProgram::GetAttributeByName(const std::string& name) const
{
   return m_attributes.at(name);
}


//-----------------------------------------------------------------------------------------------
void GPUProgram::AddUniformBindPosition(const std::string& name, const GLint& uniformBindPos)
{
   m_uniformBindPositions[name] = uniformBindPos;
}


//-----------------------------------------------------------------------------------------------
GLint GPUProgram::GetUniformBindPositionByName(const std::string& name) const
{
   auto uniformIter = m_uniformBindPositions.find(name);
   //ASSERT_OR_DIE(uniformIter != m_uniformBindPositions.end(), Stringf("Shader program does not contain uniform with name: %s", name.c_str()));

   if (uniformIter == m_uniformBindPositions.end())
   {
      return 0;
   }
   return uniformIter->second;
}