#include "Engine/Renderer/MeshRenderer.hpp"
#include "Engine/Renderer/TheRenderer.hpp"


//-----------------------------------------------------------------------------------------------
MeshRenderer::~MeshRenderer()
{
   g_theRenderer->DeleteVBO(m_vaoID);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::DeleteMaterial()
{
   delete m_material;
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::DeleteMesh()
{
   delete m_mesh;
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetMaterial(Material* mat)
{
   m_material = mat;
   g_theRenderer->BindMeshToVAO(m_vaoID, m_mesh->GetVBO(), m_mesh->GetIBO(), m_material->GetProgram());
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::Render() const
{
   g_theRenderer->DrawMeshRenderer(this);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::AddTexture(const std::string& name, Texture* tex)
{
   m_material->AddTexture(name, tex);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetVec3(const std::string& name, const Vector3f& vec)
{
   m_material->SetVec3(name, vec);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetVec4(const std::string& name, const Vector4f& vec)
{
   m_material->SetVec4(name, vec);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetMat4(const std::string& name, const Matrix4x4& mat)
{
   m_material->SetMat4(name, mat);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetInt(const std::string& name, int input)
{
   m_material->SetInt(name, input);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetFloat(const std::string& name, float input)
{
   m_material->SetFloat(name, input);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetColor(const std::string& name, const Rgba& color)
{
   m_material->SetColor(name, color);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetMultiVec3(const std::string& name, int count, const Vector3f* vecs)
{
   m_material->SetMultiVec3(name, count, vecs);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetMultiVec4(const std::string& name, int count, const Vector4f* vecs)
{
   m_material->SetMultiVec4(name, count, vecs);

}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetMultiMat4(const std::string& name, int count, const Matrix4x4* mats)
{
   m_material->SetMultiMat4(name, count, mats);

}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetMultiInt(const std::string& name, int count, int* inputs)
{
   m_material->SetMultiInt(name, count, inputs);

}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetMultiFloat(const std::string& name, int count, float* inputs)
{
   m_material->SetMultiFloat(name, count, inputs);

}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetMultiColor(const std::string& name, int count, const Rgba* colors)
{
   m_material->SetMultiColor(name, count, colors);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetLight(const std::string& name, const Light* light)
{
   m_material->SetLight(name, light);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetMultiLights(const std::string& name, int count, const Light* lights)
{
   m_material->SetMultiLights(name, count, lights);
}


//-----------------------------------------------------------------------------------------------
void MeshRenderer::SetMatrices(const Matrix4x4& model, const Matrix4x4& view, const Matrix4x4& projection)
{
   m_material->SetPositionMatrices(model, view, projection);
}