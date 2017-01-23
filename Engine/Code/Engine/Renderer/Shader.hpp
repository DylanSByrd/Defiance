#pragma once

#include <string>
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
class Shader
{
public:
   Shader(GLuint id, const std::string& name) : m_shaderID(id), m_fileName(name) {}

   GLuint GetID() const { return m_shaderID; }
   const std::string& GetFileName() const { return m_fileName; }

private:
   GLuint m_shaderID;
   std::string m_fileName;
};