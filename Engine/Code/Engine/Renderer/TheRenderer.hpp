#pragma once

#include <vector>
#include <string>
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/GPUProgram.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector3f.hpp"
#include "Engine/Renderer/VertexDefinition.hpp"


//-----------------------------------------------------------------------------------------------
// #TODO - Draw text centered at
// #TODO - Uniform drawing
// #TODO - BIG ANUS FONT RENDERER


//-----------------------------------------------------------------------------------------------
class Vector2f;
class Vector4f;
class Matrix4x4;
class Texture;
class BitmapFont;
class Shader;
class Mesh;
class MeshRenderer;
class Framebuffer;
struct RenderState;
enum BlendFactor;
enum CullMode;
enum FillMode;


//-----------------------------------------------------------------------------------------------
class TheRenderer;
extern TheRenderer* g_theRenderer;


//-----------------------------------------------------------------------------------------------
class TheRenderer
{
public:
   TheRenderer() : m_defaultMaterial(nullptr) , m_fbo(nullptr) {}
   ~TheRenderer();

   void BindMeshToVAO(GLuint vao, GLuint vbo, GLuint ibo, const GPUProgram& program) const;
   void BindGPUProgramAttributes(const GPUProgram& program) const;
   void BindMaterialUniforms(const Material* mat) const;
   GLuint RenderBufferCreate(void* data, int count, int elementSize, BufferDataUsage usage) const;
   void RenderBufferUpdate(GLuint bufferID, void* data, int count, int elementSize, BufferDataUsage usage) const;
   GLuint CreateSampler(GLenum minFilter, GLenum magFilter, GLenum uWrap, GLenum vWrap) const;

   bool Init();
   void EnableRendererDefaultCapabilities() const;
   void BindDefaultRenderState() const;
   void EnableModernGLFunctions() const;
   void CreateDefaultTexture() const;
   bool LoadDefaultMaterial();
   void InitStandardVertexDefinitions();
   Material* MakeMaterial(const std::string& vertShaderName, const std::string& fragShaderName, const RenderState& state, const VertexDefinition& vertDef) const;
   void ShaderProgramGetAttributeInfo(GPUProgram* program, const VertexDefinition& vertDef) const;
   void ShaderProgramGetUniformInfo(GPUProgram* program) const;


   void SetOrtho(const Vector2f& bottomLeft, const Vector2f& topRight) const;
   void SetPerspective(float fovDegreesY, float aspect, float nearDist, float farDist);
   GLuint GenerateVBO() const;
   void BindVBO(GLuint vboID) const;
   void SetVBOData(GLuint sizeOfVertexData, const Vertex_PCT* vertexData) const;
   void DeleteVBO(GLuint vboID) const;
   void TranslateView(const Vector2f& translation) const;
   void TranslateView(const Vector3f& translation) const;
   void RotateView(float degrees) const;
   void RotateView(float degrees, float axisX, float axisY, float axisZ) const;
   void UniformScaleView(float uniformScale) const;
   void ScaleView(Vector3f scaleFactors) const;
   void PushView() const;
   void PopView() const;
   void DrawPoint(const Vector2f& pos, const Rgba& color, float pointThickness = 1.f) const;
   void DrawPoint(const Vector3f& pos, const Rgba& color, float pointThickness = 1.f) const;
   void DrawLine(const Vector2f& startPos, const Vector2f& endPos, const Rgba& startColor, const Rgba& endColor, float lineThickness = 1.f) const;
   void DrawLine(const Vector2f& startPos, const Vector2f& endPos, const Rgba& color, float lineThickness = 1.f) const;
   void DrawLine(const Vector3f& startPos, const Vector3f& endPos, const Rgba& startColor, const Rgba& endColor, float lineThickness = 1.f) const;
   void DrawLine(const Vector3f& startPos, const Vector3f& endPos, const Rgba& color, float lineThickness = 1.f) const;
   void DrawAABB2(const AABB2& bounds, const Rgba& color, const Texture* texture = nullptr, TexBounds texBounds = TexBounds(TexCoords::ZERO, TexCoords::ONE)) const;
   void DrawQuad(const Vector2f& botLeft, const Vector2f& botRight, const Vector2f& topLeft, const Vector2f& topRight, const Rgba& color, const Texture* texture = nullptr, TexBounds texBounds = TexBounds(TexCoords::ZERO, TexCoords::ONE)) const;
   void ClearScreen(const Rgba& backgroundColor) const;
   void ClearScreen(const Rgba& backgroundColor, float depthClear) const;
   void EnableRenderCapability(int capability) const;
   
   void UseMaterial(const Material* mat) const;
   void BindRenderState(const RenderState& state) const;
   void SetBlendFunc(BlendFactor source, BlendFactor ) const;
   GLenum GetEnumForBlendFactor(BlendFactor factor) const;
   void SetCullMode(CullMode mode) const;
   void SetFillMode(FillMode mode) const;
   void SetWindingOrder(WindingOrder order) const;
   void SetDepthFunc(DepthFunc func) const;
   void EnableDepthTesting() const;
   void DisableDepthTesting() const;
   void EnableBackFaceCulling() const;
   void DisableCulling() const;
   void EnableWireFrameFilling() const;
   Mesh* CreateMesh(const std::vector<Vertex_PCT>& verts, const std::vector<GLuint>& indices) const;
   Mesh* CreateMesh(const std::vector<Vertex_PCUTB>& verts, const std::vector<GLuint>& indices) const;
   MeshRenderer* CreateMeshRenderer(Mesh* mesh, Material* mat) const;
   MeshRenderer* CreateMeshRenderer(Mesh* mesh) const;

   void SetLineWidth(float width) const;
   void SetPointSize(float size) const;
   void DrawOriginAxis(float length = 1.f, float lineThickness = 1.f, unsigned char alpha = 1, bool isDepthTesting = true) const;
   float CalcTextWidth(const std::string& text, float cellHeight, const BitmapFont* font = nullptr) const;
   void DrawText2D(const Vector2f& startPositionTopLeft, const std::string& content, const Rgba& color, float cellHeight, const BitmapFont* font = nullptr, const Vector2f& upDir = Vector2f(0.f, 1.f), const Vector2f& rightDir = Vector2f(1.f, 0.f)) const;
   void DrawText3D(const Vector3f& startPositionTopLeft, const std::string& content, const Rgba& color, float cellHeight, const BitmapFont* font = nullptr, const Vector3f& upDir = Vector3f(0.f, 1.f, 0.f), const Vector3f& rightDir = Vector3f(1.f, 0.f, 0.f)) const;
   void Draw3DQuad(const Vector3f& botLeft, const Vector3f& botRight, const Vector3f& topRight, const Vector3f& topLeft, const Rgba& tint = Rgba::WHITE, const Texture* texture = nullptr, TexBounds texBounds = TexBounds(TexCoords::ZERO, TexCoords::ONE)) const;
   void EnableContrastBlending() const;
   void BindTexture(const Texture& texture) const;
   void DrawHollowQuad(const Vector2f& botLeft, const Vector2f& botRight, const Vector2f& topRight, const Vector2f& topLeft, const Rgba& tint = Rgba::WHITE, float thickness = 1.f) const;
   void DrawHollowQuad(const Vector3f& botLeft, const Vector3f& botRight, const Vector3f& topRight, const Vector3f& topLeft, const Rgba& tint = Rgba::WHITE, float thickness = 1.f) const;
   //void DrawHollowCircle(const Vector3f& centerPos, float radius, const Vector3f& axis, const Rgba& tint = Rgba::WHITE, int numSides = 50) const;
   void DrawVertexArray_PCT(const Vertex_PCT* const vertexes, int numVertexes, PrimitiveType primitiveType) const;
   void DrawVBO_PCT(unsigned int vboID, unsigned int numVertexes, PrimitiveType primitiveType) const;
   
   
   Shader LoadShader(const std::string& shaderName, GLuint shaderType) const;
   GPUProgram CreateAndLinkProgram(const Shader& vertShader, const Shader& fragShader) const;
   void DeleteProgram(const GPUProgram& program) const;
   void LogShaderError(const std::string& filePath, GLuint id, bool isProgram) const;
   void MakeShaderErrorWindow(const std::string& filePath, const std::string& errorLog) const;
   std::string GetOpenGLVersion() const;
   std::string GetGLSLVersion() const;

   void DrawMeshRenderer(const MeshRenderer* meshRenderer) const;
   void DrawMesh(const Mesh* mesh) const;
   const Material* GetDefaultMaterial() const { return m_defaultMaterial; }
   // #TODO - put into a mesh builder
   Mesh* GenerateSphereMeshPCT(float radius, int slices, int stacks) const;
   Mesh* GenerateSphereMeshPCUTB(float radius, int slices, int stacks) const;
   Mesh* GenerateCubeMeshPCT(float length, float width, float height) const;
   Mesh* GenerateCubeMeshPCUTB(float length, float width, float height) const;
   Mesh* GenerateCubeMeshPCUTBN(float length, float width, float height) const;
   Mesh* GenerateCubeMeshBONE(float length, float width, float height) const;
   Mesh* GenerateQuadMesh(float width, float height) const;

   Framebuffer* CreateFramebuffer(int width, int height, const std::vector<TextureFormat>& colorFormats, TextureFormat depthStencilFormat);
   void DeleteFramebuffer(Framebuffer* fbo);
   void BindFrameBuffer(Framebuffer* fbo);
   void CopyFramebufferToBack(Framebuffer* fbo) const;

   const VertexDefinition& GetPCTVertDef() const { return m_vertexPCT; }
   const VertexDefinition& GetPCUTBVertDef() const { return m_vertexPCUTB; }
   const VertexDefinition& GetPCUTBNVertDef() const { return m_vertexPCUTBN; }
   const VertexDefinition& GetBoneVertDef() const { return m_vertexBone; }
   const Vector3f& GetWBufferParams() const { return m_wBufferParams; }
   GLenum GetEnumForBufferDataUsage(BufferDataUsage usage) const;

   static GLuint UNBIND_BUFFER_ID;
   static const char* DEFAULT_TEXTURE_NAME;
   static const char* DEFAULT_FONT_NAME;

private:
   Framebuffer* m_fbo;
   Material* m_defaultMaterial;
   VertexDefinition m_vertexPCT;
   VertexDefinition m_vertexPCUTB;
   VertexDefinition m_vertexPCUTBN;
   VertexDefinition m_vertexBone;
   Vector3f m_wBufferParams;
};