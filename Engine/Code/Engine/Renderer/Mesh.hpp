#pragma once

#include <vector>
#include "Engine/Renderer/RendererCommon.hpp"
#include "Engine/Renderer/DrawInstruction.hpp"


//-----------------------------------------------------------------------------------------------
class VertexDefinition;


//-----------------------------------------------------------------------------------------------
class Mesh
{
public:
   Mesh(GLuint vbo, GLuint ibo, const VertexDefinition* vertDef) : m_vbo(vbo), m_ibo(ibo), m_vertDef(vertDef) {}
   ~Mesh();

   void WriteToVBO(unsigned char* vertexBuffer, unsigned int vertexCount);
   void WriteToIBO(const std::vector<int>& indexBuffer);
   void SetDrawInstructions(const std::vector<DrawInstruction>& newInstructions);

   void SetVBO(GLuint vbo) { m_vbo = vbo; }
   void SetIBO(GLuint ibo) { m_ibo = ibo; }

   GLuint GetVBO() const { return m_vbo; }
   GLuint GetIBO() const { return m_ibo; }
   const VertexDefinition* GetVertexDefinition() const { return m_vertDef; }
   const std::vector<DrawInstruction>& GetDrawInstructions() const { return m_drawInstructions; }

private:
   GLuint m_vbo;
   GLuint m_ibo;
   const VertexDefinition* m_vertDef;
   std::vector<DrawInstruction> m_drawInstructions;
};