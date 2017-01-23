#pragma once

#include <map>
#include <string>
#include "Engine/Renderer/RendererCommon.hpp"


//-----------------------------------------------------------------------------------------------
struct GPUProgramAttribute
{
   int elementCount;
   ElementType type;
   GLint bindPos;
   bool isNormalized;
   GLsizei stride;
   GLvoid* offset = nullptr;
};


//-----------------------------------------------------------------------------------------------
class GPUProgram
{
public:
   GPUProgram(GLuint id) : m_programID(id), m_attributes() {}
   ~GPUProgram() {}

   GLuint GetID() const { return m_programID; }
   bool IsValid() const { return m_programID != NULL; }

   void AddAttribute(const std::string& name, const GPUProgramAttribute& attribute);
   const GPUProgramAttribute& GetAttributeByName(const std::string& name) const;
   int GetNumAttributes() const { return m_attributes.size(); }
   const std::map<std::string, GPUProgramAttribute>& GetAttributes() const { return m_attributes; }

   void AddUniformBindPosition(const std::string& name, const GLint& uniformBindPos);
   GLint GetUniformBindPositionByName(const std::string& name) const;
   int GetNumUniforms() const { return m_uniformBindPositions.size(); }
   const std::map<std::string, GLint>& GetUniformsBindPositions() const { return m_uniformBindPositions; }


private:
   GLuint m_programID;
   std::map<std::string, GPUProgramAttribute> m_attributes;
   std::map<std::string,  GLint> m_uniformBindPositions;
};