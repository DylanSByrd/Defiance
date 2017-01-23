#pragma once

#include "Engine/Renderer/GPUProgram.hpp"
#include "Engine/Renderer/RenderState.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/VertexDefinition.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Vector3f.hpp"
#include "Engine/Math/Vector4f.hpp"


//-----------------------------------------------------------------------------------------------
struct Light;


//-----------------------------------------------------------------------------------------------
struct UniformData
{
   int elementCount = 0;
   ElementType type = INVALID_TYPE;
   GLint bindPos = 0;
   GLvoid* data = nullptr;
};


//-----------------------------------------------------------------------------------------------
class Material
{
public:
   Material(const GPUProgram& program, const RenderState& state, const VertexDefinition& vertDef) : m_program(program), m_renderState(state), m_vertDef(vertDef), m_samplerID(0) {}
   ~Material();

   const GPUProgram& GetProgram() const { return m_program; }
   GLuint GetProgramID() const { return m_program.GetID(); }
   const RenderState& GetRenderState() const { return m_renderState; }
   const std::map<std::string, const Texture*>& GetTextures() const { return m_textures; }
   int GetTextureCount() const { return m_textures.size(); }
   GLuint GetSamplerID() const { return m_samplerID; }
   void SetSamplerID(GLuint id) { m_samplerID = id; }

   void ClearAllUniformData();
   UniformData* CreateOrGetUniformData(const std::string& name);
   const UniformData* GetUniformByName(const std::string& name) const;
   const std::map<std::string, UniformData*>& GetUniforms() const { return m_uniforms; }

   void AddTexture(const std::string& name, const Texture* tex);
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

   void SetLight(const std::string& name, const Light* light);
   void SetMultiLights(const std::string& name, int count, const Light* lights);

   void SetPositionMatrices(const Matrix4x4& model, const Matrix4x4& view, const Matrix4x4& projection);

private:
   GPUProgram m_program;
   RenderState m_renderState;
   VertexDefinition m_vertDef;
   GLuint m_samplerID;
   std::map<std::string, const Texture*> m_textures;
   std::map<std::string, UniformData*> m_uniforms;
};