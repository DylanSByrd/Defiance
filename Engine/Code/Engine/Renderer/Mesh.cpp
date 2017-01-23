#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/TheRenderer.hpp"


//-----------------------------------------------------------------------------------------------
Mesh::~Mesh()
{
   g_theRenderer->DeleteVBO(m_vbo);
   g_theRenderer->DeleteVBO(m_ibo);
}


//-----------------------------------------------------------------------------------------------
void Mesh::WriteToVBO(unsigned char* vertexBuffer, unsigned int vertexCount)
{
   g_theRenderer->RenderBufferUpdate(m_vbo, vertexBuffer, vertexCount, m_vertDef->GetSizeOfVertex(), STATIC_DRAW);
}


//-----------------------------------------------------------------------------------------------
void Mesh::WriteToIBO(const std::vector<int>& indexBuffer)
{
   if (indexBuffer.size() != 0)
   {
      g_theRenderer->RenderBufferUpdate(m_ibo, (void*)&indexBuffer[0], indexBuffer.size(), sizeof(indexBuffer[0]), STATIC_DRAW);
   }
}


//-----------------------------------------------------------------------------------------------
void Mesh::SetDrawInstructions(const std::vector<DrawInstruction>& newInstructions)
{
   m_drawInstructions.clear();
   m_drawInstructions.resize(newInstructions.size());
   m_drawInstructions.assign(newInstructions.begin(), newInstructions.end());
}