#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Mesh.hpp"


//-----------------------------------------------------------------------------------------------
class MeshRenderer
{
public:
   MeshRenderer(GLuint vaoID, Mesh* mesh, Material* mat) : m_vaoID(vaoID), m_mesh(mesh), m_material(mat) {}
   ~MeshRenderer();
   void DeleteMaterial();
   void DeleteMesh();

   GLuint GetVAO() const { return m_vaoID; }
   const Mesh* GetMesh() const { return m_mesh; }
   const Material* GetMaterial() const { return m_material; }
   void SetMaterial(Material* mat);
   void Render() const;

   void AddTexture(const std::string& name, Texture* tex);
   void SetVec3(const std::string& name, const Vector3f& vec);
   void SetVec4(const std::string& name, const Vector4f& vec);
   void SetMat4(const std::string& name, const Matrix4x4& mat);
   void SetInt(const std::string& name, int input);
   void SetFloat(const std::string& name, float input);
   void SetColor(const std::string& name, const Rgba& color);

   void SetMultiVec3(const std::string& name, int count, const Vector3f* vecs);
   void SetMultiVec4(const std::string& name, int count, const Vector4f* vecs);
   void SetMultiMat4(const std::string& name, int count, const Matrix4x4* mats);
   void SetMultiInt(const std::string& name, int count, int* inputs);
   void SetMultiFloat(const std::string& name, int count, float* inputs);
   void SetMultiColor(const std::string& name, int count, const Rgba* colors);

   // add set light to both this and material
   void SetLight(const std::string& name, const Light* light);
   void SetMultiLights(const std::string& name, int count, const Light* lights);


   void SetMatrices(const Matrix4x4& model, const Matrix4x4& view, const Matrix4x4& projection);

private:
   GLuint m_vaoID;
   Mesh* m_mesh;
   Material* m_material;
};