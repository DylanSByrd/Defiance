#include <cmath>
#include <vector>
#include <sstream>
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/OpenGLExtensions.hpp"
#include "Engine/Renderer/RenderState.hpp"
#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshRenderer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Math/Vector2f.hpp"
#include "Engine/Math/Vector3f.hpp"
#include "Engine/Math/Vector4f.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/IO/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Debug/Console.hpp"

#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#pragma comment( lib, "GLu32" ) // Link to GLu static library

// #TODO - let the engine handle app shutdown

//-----------------------------------------------------------------------------------------------
extern TheRenderer* g_theRenderer = nullptr;


//-----------------------------------------------------------------------------------------------
// #TODO - put in shutdown func
TheRenderer::~TheRenderer()
{
   if (m_defaultMaterial != nullptr)
   {
      delete m_defaultMaterial;
   }
}


//-----------------------------------------------------------------------------------------------
STATIC GLuint TheRenderer::UNBIND_BUFFER_ID = 0;
STATIC const char* TheRenderer::DEFAULT_TEXTURE_NAME = "Default Texture";
STATIC const char* TheRenderer::DEFAULT_FONT_NAME = "SquirrelFixedFont";


void TheRenderer::BindMeshToVAO(GLuint vao, GLuint vbo, GLuint ibo, const GPUProgram& program) const
{
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);

   BindGPUProgramAttributes(program);

   glBindBuffer(GL_ARRAY_BUFFER, NULL);
   
   if (NULL != ibo)
   {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
   }

   glBindVertexArray(NULL);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::BindGPUProgramAttributes(const GPUProgram& program) const
{
   GLint programID = program.GetID();
   glUseProgram(programID);

   const std::map<std::string, GPUProgramAttribute>& attributes = program.GetAttributes();
   for (auto attributeIter = attributes.begin(); attributeIter != attributes.end(); ++attributeIter)
   {
      const GPUProgramAttribute& attribute = attributeIter->second;
      GLint pos = attribute.bindPos;
      if (pos < 0)
      {
         continue;
      }

      // add cases as needed
      GLenum elementType = GL_FLOAT;
      switch (attribute.type)
      {
      case FLOAT_ELEMENT:
      {
         elementType = GL_FLOAT;
         break;
      }

      case UNSIGNED_BYTE_ELEMENT:
      {
         elementType = GL_UNSIGNED_BYTE;
         break;
      }

      case INT_ELEMENT:
      {
         elementType = GL_UNSIGNED_INT;
         break;
      }
      }

      if (elementType != GL_UNSIGNED_INT)
      {
         glEnableVertexAttribArray(pos);
         glVertexAttribPointer(pos, // bind point to shader
            attribute.elementCount, //num elements
            elementType, // type of element
            attribute.isNormalized, //normalize data for me
            attribute.stride, //stride
            attribute.offset); //offset of member
      }
      else
      {
         glEnableVertexAttribArray(pos);
         glVertexAttribIPointer(pos, // bind point to shader
            attribute.elementCount, //num elements
            elementType, // type of element
            attribute.stride, //stride
            attribute.offset); //offset of member
      }
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::BindMaterialUniforms(const Material* mat) const
{
   glUseProgram(mat->GetProgramID());
   const std::map<std::string, UniformData*>& uniforms = mat->GetUniforms();

   for (auto uniformIter = uniforms.begin(); uniformIter != uniforms.end(); ++uniformIter)
   {
      const UniformData* currentUniform = uniformIter->second;
      if (currentUniform->data == nullptr)
      {
         continue;
      }

      GLint bindPos = currentUniform->bindPos;
      if (bindPos >= 0)
      {
         switch (currentUniform->type)
         {
         case FLOAT_ELEMENT:
         {
            glUniform1fv(bindPos, currentUniform->elementCount, (GLfloat*)(currentUniform->data));
            break;
         }

         case INT_ELEMENT:
         {
            glUniform1iv(bindPos, currentUniform->elementCount, (GLint*)(currentUniform->data));
            break;
         }

         case VEC3_ELEMENT:
         {
            glUniform3fv(bindPos, currentUniform->elementCount, (GLfloat*)currentUniform->data);
            break;
         }

         case VEC4_ELEMENT:
         {
            glUniform4fv(bindPos, currentUniform->elementCount, (GLfloat*)currentUniform->data);
            break;
         }

         case MAT4_ELEMENT:
         {
            glUniformMatrix4fv(bindPos, currentUniform->elementCount, GL_FALSE, (GLfloat*)currentUniform->data);
            break;
         }
         }
      }
   }
}


//-----------------------------------------------------------------------------------------------
GLuint TheRenderer::RenderBufferCreate(void* data, int count, int elementSize, BufferDataUsage usage) const
{
   GLuint buffer;
   glGenBuffers(1, &buffer);

   GLenum glUsage = GetEnumForBufferDataUsage(usage);

   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferData(GL_ARRAY_BUFFER, count * elementSize, data, glUsage);
   glBindBuffer(GL_ARRAY_BUFFER, NULL);

   return buffer;
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::RenderBufferUpdate(GLuint bufferID, void* data, int count, int elementSize, BufferDataUsage usage) const
{
   GLenum glUsage = GetEnumForBufferDataUsage(usage);

   glBindBuffer(GL_ARRAY_BUFFER, bufferID);
   glBufferData(GL_ARRAY_BUFFER, count * elementSize, data, glUsage);
   glBindBuffer(GL_ARRAY_BUFFER, NULL);
}


//-----------------------------------------------------------------------------------------------
GLuint TheRenderer::CreateSampler(GLenum minFilter, GLenum magFilter, GLenum uWrap, GLenum vWrap) const
{
   GLuint id;
   glGenSamplers(1, &id);

   glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, minFilter);
   glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, magFilter);
   glSamplerParameteri(id, GL_TEXTURE_WRAP_S, uWrap);
   glSamplerParameteri(id, GL_TEXTURE_WRAP_T, vWrap);

   return id;
}


//-----------------------------------------------------------------------------------------------
bool TheRenderer::Init()
{
   EnableRendererDefaultCapabilities();
   BindDefaultRenderState();
   EnableModernGLFunctions();
   InitStandardVertexDefinitions();
   CreateDefaultTexture();
   
   // #TODO - getters
   float nearDist = .1f;
   float farDist = 1000.f;
   float depthScale = 1.f / (farDist - nearDist);
   m_wBufferParams = Vector3f(2.f * depthScale, -(farDist + nearDist) * depthScale, 0.f);
   
   if (!LoadDefaultMaterial())
   {
      return false;
   }

   return true;
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::EnableRendererDefaultCapabilities() const
{
   EnableRenderCapability(GL_BLEND);
   EnableRenderCapability(GL_LINE_SMOOTH);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::BindDefaultRenderState() const
{
   RenderState defaultState = { CULL_BACK, SOURCE_ALPHA, INVERT_SOURCE_ALPHA, FILL_FULL, WIND_CCW, WRITE_LESS, false };
   BindRenderState(defaultState);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::EnableModernGLFunctions() const
{
   glGenBuffers              = (PFNGLGENBUFFERSPROC)              wglGetProcAddress("glGenBuffers");
   glDeleteBuffers           = (PFNGLDELETEBUFFERSPROC)           wglGetProcAddress("glDeleteBuffers");
   glBindBuffer              = (PFNGLBINDBUFFERPROC)              wglGetProcAddress("glBindBuffer");
   glBufferData              = (PFNGLBUFFERDATAPROC)              wglGetProcAddress("glBufferData");
   glCreateShader            = (PFNGLCREATESHADERPROC)            wglGetProcAddress("glCreateShader");
   glShaderSource            = (PFNGLSHADERSOURCEPROC)            wglGetProcAddress("glShaderSource");
   glCompileShader           = (PFNGLCOMPILESHADERPROC)           wglGetProcAddress("glCompileShader");
   glGetShaderiv             = (PFNGLGETSHADERIVPROC)             wglGetProcAddress("glGetShaderiv");
   glDeleteShader            = (PFNGLDELETESHADERPROC)            wglGetProcAddress("glDeleteShader");
   glCreateProgram           = (PFNGLCREATEPROGRAMPROC)           wglGetProcAddress("glCreateProgram");
   glAttachShader            = (PFNGLATTACHSHADERPROC)            wglGetProcAddress("glAttachShader");
   glLinkProgram             = (PFNGLLINKPROGRAMPROC)             wglGetProcAddress("glLinkProgram");
   glGetProgramiv            = (PFNGLGETPROGRAMIVPROC)            wglGetProcAddress("glGetProgramiv");
   glDetachShader            = (PFNGLDETACHSHADERPROC)            wglGetProcAddress("glDetachShader");
   glDeleteProgram           = (PFNGLDELETEPROGRAMPROC)           wglGetProcAddress("glDeleteProgram");
   glGetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC)        wglGetProcAddress("glGetShaderInfoLog");
   glGetProgramInfoLog       = (PFNGLGETPROGRAMINFOLOGPROC)       wglGetProcAddress("glGetProgramInfoLog");
   glGenVertexArrays         = (PFNGLGENVERTEXARRAYSPROC)         wglGetProcAddress("glGenVertexArrays");
   glDeleteVertexArrays      = (PFNGLDELETEVERTEXARRAYSPROC)      wglGetProcAddress("glDeleteVertexArrays");
   glBindVertexArray         = (PFNGLBINDVERTEXARRAYPROC)         wglGetProcAddress("glBindVertexArray");
   glGetAttribLocation       = (PFNGLGETATTRIBLOCATIONPROC)       wglGetProcAddress("glGetAttribLocation");
   glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glEnableVertexAttribArray");
   glVertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC)     wglGetProcAddress("glVertexAttribPointer");
   glVertexAttribIPointer    = (PFNGLVERTEXATTRIBIPOINTERPROC)    wglGetProcAddress("glVertexAttribIPointer");
   glUseProgram              = (PFNGLUSEPROGRAMPROC)              wglGetProcAddress("glUseProgram");
   glGetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC)      wglGetProcAddress("glGetUniformLocation");
   glUniform1fv              = (PFNGLUNIFORM1FVPROC)              wglGetProcAddress("glUniform1fv");
   glUniform2fv              = (PFNGLUNIFORM2FVPROC)              wglGetProcAddress("glUniform2fv");
   glUniform3fv              = (PFNGLUNIFORM3FVPROC)              wglGetProcAddress("glUniform3fv");
   glUniform4fv              = (PFNGLUNIFORM4FVPROC)              wglGetProcAddress("glUniform4fv");
   glUniform1iv              = (PFNGLUNIFORM1IVPROC)              wglGetProcAddress("glUniform1iv");
   glUniform2iv              = (PFNGLUNIFORM2IVPROC)              wglGetProcAddress("glUniform2iv");
   glUniform3iv              = (PFNGLUNIFORM3IVPROC)              wglGetProcAddress("glUniform3iv");
   glUniform4iv              = (PFNGLUNIFORM4IVPROC)              wglGetProcAddress("glUniform4iv");
   glUniformMatrix4fv        = (PFNGLUNIFORMMATRIX4FVPROC)        wglGetProcAddress("glUniformMatrix4fv");
   glGenSamplers             = (PFNGLGENSAMPLERSPROC)             wglGetProcAddress("glGenSamplers");
   glSamplerParameteri       = (PFNGLSAMPLERPARAMETERIPROC)       wglGetProcAddress("glSamplerParameteri");
   glActiveTexture           = (PFNGLACTIVETEXTUREPROC)           wglGetProcAddress("glActiveTexture");
   glBindSampler             = (PFNGLBINDSAMPLERPROC)             wglGetProcAddress("glBindSampler");
   glUniform1i               = (PFNGLUNIFORM1IPROC)               wglGetProcAddress("glUniform1i");
   glUniform1f               = (PFNGLUNIFORM1FPROC)               wglGetProcAddress("glUniform1f");
   glGetActiveAttrib         = (PFNGLGETACTIVEATTRIBPROC)         wglGetProcAddress("glGetActiveAttrib");
   glGetActiveUniform        = (PFNGLGETACTIVEUNIFORMPROC)        wglGetProcAddress("glGetActiveUniform");
   glGenFramebuffers         = (PFNGLGENFRAMEBUFFERSPROC)         wglGetProcAddress("glGenFramebuffers");
   glBindFramebuffer         = (PFNGLBINDFRAMEBUFFERPROC)         wglGetProcAddress("glBindFramebuffer");
   glFramebufferTexture      = (PFNGLFRAMEBUFFERTEXTUREPROC)      wglGetProcAddress("glFramebufferTexture");
   glCheckFramebufferStatus  = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)  wglGetProcAddress("glCheckFramebufferStatus");
   glDeleteFramebuffers      = (PFNGLDELETEFRAMEBUFFERSPROC)      wglGetProcAddress("glDeleteFramebuffers");
   glDrawBuffers             = (PFNGLDRAWBUFFERSPROC)             wglGetProcAddress("glDrawBuffers");
   glBlitFramebuffer         = (PFNGLBLITFRAMEBUFFERPROC)         wglGetProcAddress("glBlitFramebuffer");
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::CreateDefaultTexture() const
{
   unsigned char plainWhiteTexel[4] = { 255, 255, 255, 255 };
   Texture::CreateTextureFromData(plainWhiteTexel, Vector2i::ONE, DEFAULT_TEXTURE_NAME);

   // Make sure it worked!
   if (Texture::GetTextureByName(DEFAULT_TEXTURE_NAME) == nullptr)
   {
      ERROR_AND_DIE("Default texture failed to load.");
   }
}


//-----------------------------------------------------------------------------------------------
bool TheRenderer::LoadDefaultMaterial()
{
   RenderState defaultState = { CULL_BACK, SOURCE_ALPHA, INVERT_SOURCE_ALPHA, FILL_FULL, WIND_CCW, WRITE_LESS, false };
   m_defaultMaterial = MakeMaterial("fvf.vert", "fvf.frag", defaultState, m_vertexPCT);
   if (m_defaultMaterial == nullptr)
   {
      return false;
   }

   m_defaultMaterial->AddTexture("uTexDiffuse", Texture::CreateOrGetTexture(DEFAULT_TEXTURE_NAME));
   m_defaultMaterial->SetColor("uColor", Rgba::WHITE);
   m_defaultMaterial->SetVec3("uWBufferParams", m_wBufferParams);

   return true;
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::InitStandardVertexDefinitions()
{
   // PCT definition
   VertexComponent pctPosition("inPosition", VertexComponent::POSITION, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_PCT, position));
   VertexComponent pctColor("inColor", VertexComponent::COLOR, 4, UNSIGNED_BYTE_ELEMENT, true, (GLvoid*)offsetof(Vertex_PCT, color));
   VertexComponent pctUV("inUV0", VertexComponent::UV, 2, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_PCT, texCoords));
   m_vertexPCT.AddComponent(pctPosition);
   m_vertexPCT.AddComponent(pctColor);
   m_vertexPCT.AddComponent(pctUV);

   // PCUTB definition
   VertexComponent pcutbPosition("inPosition", VertexComponent::POSITION, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_PCUTB, position));
   VertexComponent pcutbColor("inColor", VertexComponent::COLOR, 4, UNSIGNED_BYTE_ELEMENT, true, (GLvoid*)offsetof(Vertex_PCUTB, color));
   VertexComponent pcutbUV("inUV0", VertexComponent::UV, 2, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_PCUTB, uv));
   VertexComponent pcutbTan("inTangent", VertexComponent::TANGENT, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_PCUTB, tangent));
   VertexComponent pcutbBit("inBitangent", VertexComponent::BITANGENT, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_PCUTB, bitangent));
   m_vertexPCUTB.AddComponent(pcutbPosition);
   m_vertexPCUTB.AddComponent(pcutbColor);
   m_vertexPCUTB.AddComponent(pcutbUV);
   m_vertexPCUTB.AddComponent(pcutbTan);
   m_vertexPCUTB.AddComponent(pcutbBit);

   // PCUTBN definition
   VertexComponent pcutbnPosition("inPosition", VertexComponent::POSITION, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_PCUTBN, position));
   VertexComponent pcutbnColor("inColor", VertexComponent::COLOR, 4, UNSIGNED_BYTE_ELEMENT, true, (GLvoid*)offsetof(Vertex_PCUTBN, color));
   VertexComponent pcutbnUV("inUV0", VertexComponent::UV, 2, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_PCUTBN, uv));
   VertexComponent pcutbnTan("inTangent", VertexComponent::TANGENT, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_PCUTBN, tangent));
   VertexComponent pcutbnBit("inBitangent", VertexComponent::BITANGENT, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_PCUTBN, bitangent));
   VertexComponent pcutbnNormal("inNormal", VertexComponent::NORMAL, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_PCUTBN, normal));
   m_vertexPCUTBN.AddComponent(pcutbnPosition);
   m_vertexPCUTBN.AddComponent(pcutbnColor);
   m_vertexPCUTBN.AddComponent(pcutbnUV);
   m_vertexPCUTBN.AddComponent(pcutbnTan);
   m_vertexPCUTBN.AddComponent(pcutbnBit);
   m_vertexPCUTBN.AddComponent(pcutbnNormal);

   // BONE definition
   VertexComponent bonePosition("inPosition", VertexComponent::POSITION, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_Bone, position));
   VertexComponent boneColor("inColor", VertexComponent::COLOR, 4, UNSIGNED_BYTE_ELEMENT, true, (GLvoid*)offsetof(Vertex_Bone, color));
   VertexComponent boneUV("inUV0", VertexComponent::UV, 2, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_Bone, uv));
   VertexComponent boneTan("inTangent", VertexComponent::TANGENT, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_Bone, tangent));
   VertexComponent boneBit("inBitangent", VertexComponent::BITANGENT, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_Bone, bitangent));
   VertexComponent boneNormal("inNormal", VertexComponent::NORMAL, 3, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_Bone, normal));
   VertexComponent boneIndices("inBoneIndices", VertexComponent::BONE_INDICES, 4, INT_ELEMENT, false, (GLvoid*)offsetof(Vertex_Bone, boneIndices));
   VertexComponent boneWeights("inBoneWeights", VertexComponent::BONE_WEIGHTS, 4, FLOAT_ELEMENT, false, (GLvoid*)offsetof(Vertex_Bone, boneWeights));
   m_vertexBone.AddComponent(bonePosition);
   m_vertexBone.AddComponent(boneColor);
   m_vertexBone.AddComponent(boneUV);
   m_vertexBone.AddComponent(boneTan);
   m_vertexBone.AddComponent(boneBit);
   m_vertexBone.AddComponent(boneNormal);
   m_vertexBone.AddComponent(boneIndices);
   m_vertexBone.AddComponent(boneWeights);
}


//-----------------------------------------------------------------------------------------------
Material* TheRenderer::MakeMaterial(const std::string& vertShaderName, const std::string& fragShaderName, const RenderState& state, const VertexDefinition& vertDef) const
{
   Shader vertShader = LoadShader(vertShaderName, GL_VERTEX_SHADER);
   Shader fragShader = LoadShader(fragShaderName, GL_FRAGMENT_SHADER);
   if (vertShader.GetID() == NULL || fragShader.GetID() == NULL)
   {
      return nullptr;
   }

   GPUProgram program = CreateAndLinkProgram(vertShader, fragShader);
   if (program.GetID() == NULL)
   {
      return nullptr;
   }

   glDeleteShader(vertShader.GetID());
   glDeleteShader(fragShader.GetID());

   ShaderProgramGetAttributeInfo(&program, vertDef);
   ShaderProgramGetUniformInfo(&program);   

   Material* newMat = new Material(program, state, vertDef);

   GLuint defaultSampler = CreateSampler(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
   newMat->SetSamplerID(defaultSampler);

   return newMat;
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::ShaderProgramGetAttributeInfo(GPUProgram* program, const VertexDefinition& vertDef) const
{
   // Find the total number of attributes
   GLuint programID = program->GetID();
   int numAttributes = 0;
   glGetProgramiv(programID, GL_ACTIVE_ATTRIBUTES, &numAttributes);

   // Max attribute name length
   int maxAttributeNameLength = 0;
   glGetProgramiv(programID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeNameLength);

   // Build and save each attribute
   GLsizei stride = vertDef.GetSizeOfVertex();
   for (int attributeIter = 0; attributeIter < numAttributes; ++attributeIter)
   {
      GPUProgramAttribute attribute;
      std::string attributeName;
      attributeName.resize(maxAttributeNameLength);
      int bytesWritten = 0;
      GLenum typeFromShader;
      glGetActiveAttrib(programID, attributeIter, maxAttributeNameLength, &bytesWritten, &attribute.elementCount, &typeFromShader, &attributeName[0]);
      attributeName.resize(bytesWritten);
      attribute.bindPos = glGetAttribLocation(programID, attributeName.c_str());

      // Update attribute with vertex definition
      const VertexComponent* component = vertDef.GetComponentWithName(attributeName);
      //ASSERT_OR_DIE(component != nullptr, Stringf("Vertex Definition does not contain component %s.", attributeName.c_str()));
      attribute.elementCount = component->count;
      attribute.type = component->type;
      attribute.isNormalized = component->isNormalized;
      attribute.stride = stride;
      attribute.offset = component->offset;


      program->AddAttribute(attributeName, attribute);
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::ShaderProgramGetUniformInfo(GPUProgram* program) const
{
   // Get num of uniforms
   GLuint programID = program->GetID();
   int numUniforms = 0;
   glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &numUniforms);

   // Max uniform name length
   int maxUniformNameLength = 0;
   glGetProgramiv(programID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);

   // Build and save each uniform
   for (int uniformIter = 0; uniformIter < numUniforms; ++uniformIter)
   {
      std::string uniformName;
      GLenum type;
      GLint size;
      uniformName.resize(maxUniformNameLength);
      int bytesWritten = 0;
      glGetActiveUniform(programID, uniformIter, maxUniformNameLength, &bytesWritten, &size, &type, &uniformName[0]);
      uniformName.resize(bytesWritten);
      GLint bindPos = glGetUniformLocation(programID, uniformName.c_str());
      program->AddUniformBindPosition(uniformName, bindPos);
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::SetOrtho(const Vector2f& bottomLeft, const Vector2f& topRight) const
{
   glLoadIdentity();
   glOrtho(bottomLeft.x, topRight.x, bottomLeft.y, topRight.y, 0.f, 1.f);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::SetPerspective(float fovDegreesY, float aspect, float nearDist, float farDist)
{
   glLoadIdentity();
   gluPerspective(fovDegreesY, aspect, nearDist, farDist);
}


//-----------------------------------------------------------------------------------------------
GLuint TheRenderer::GenerateVBO() const
{
   GLuint vboID;
   glGenBuffers(1, &vboID);
   return vboID;
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::BindVBO(GLuint vboID) const
{
   glBindBuffer(GL_ARRAY_BUFFER, vboID);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::SetVBOData(GLuint sizeOfVertexData, const Vertex_PCT* vertexData) const
{
   glBufferData(GL_ARRAY_BUFFER, sizeOfVertexData, vertexData, GL_STATIC_DRAW);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DeleteVBO(GLuint vboID) const
{
   glDeleteBuffers(1, &vboID);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::TranslateView(const Vector2f& translation) const
{
   glTranslatef(translation.x, translation.y, 0.f);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::TranslateView(const Vector3f& translation) const
{
   glTranslatef(translation.x, translation.y, translation.z);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::RotateView(float degrees) const
{
   glRotatef(degrees, 0.f, 0.f, 1.f);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::RotateView(float degrees, float axisX, float axisY, float axisZ) const
{
   glRotatef(degrees, axisX, axisY, axisZ);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::UniformScaleView(float uniformScale) const
{
   glScalef(uniformScale, uniformScale, 1.f);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::ScaleView(Vector3f scaleFactors) const
{
   glScalef(scaleFactors.x, scaleFactors.y, scaleFactors.z);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::PushView() const
{
   glPushMatrix();
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::PopView() const
{
   glPopMatrix();
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawPoint(const Vector2f& pos, const Rgba& color, float pointThickness /*= 1.f*/) const
{
   BindTexture(*(Texture::GetTextureByName(DEFAULT_TEXTURE_NAME)));
   glPointSize(pointThickness);

   Vertex_PCT pointToDraw = { Vector3f(pos.x, pos.y, 0.f), color, TexCoords::ZERO };
   DrawVertexArray_PCT(&pointToDraw, 1, PRIMITIVE_POINTS);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawPoint(const Vector3f& pos, const Rgba& color, float pointThickness /*= 1.f*/) const
{
   BindTexture(*(Texture::GetTextureByName(DEFAULT_TEXTURE_NAME)));
   glPointSize(pointThickness);

   Vertex_PCT pointToDraw = { pos, color, TexCoords::ZERO };
   DrawVertexArray_PCT(&pointToDraw, 1, PRIMITIVE_POINTS);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawLine(const Vector2f& startPos, const Vector2f& endPos, const Rgba& startColor, 
   const Rgba& endColor, float lineThickness /*= 1.f*/) const
{
   BindTexture(*(Texture::GetTextureByName(DEFAULT_TEXTURE_NAME)));
   SetLineWidth(lineThickness);

   Vertex_PCT vertexes[2];
   vertexes[0] = { Vector3f(startPos.x, startPos.y, 0.f), startColor, TexCoords::ZERO };
   vertexes[1] = { Vector3f(endPos.x, endPos.y, 0.f), endColor, TexCoords::ZERO };
   DrawVertexArray_PCT(vertexes, 2, PRIMITIVE_LINES);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawLine(const Vector2f& startPos, const Vector2f& endPos, const Rgba& color, float lineThickness /*= 1.f*/) const
{
   BindTexture(*(Texture::GetTextureByName(DEFAULT_TEXTURE_NAME)));
   SetLineWidth(lineThickness);

   Vertex_PCT vertexes[2];
   vertexes[0] = { Vector3f(startPos.x, startPos.y, 0.f), color, TexCoords::ZERO };
   vertexes[1] = { Vector3f(endPos.x, endPos.y, 0.f), color, TexCoords::ZERO };
   DrawVertexArray_PCT(vertexes, 2, PRIMITIVE_LINES);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawLine(const Vector3f& startPos, const Vector3f& endPos, const Rgba& startColor, const Rgba& endColor, float lineThickness /*= 1.f*/) const
{
   BindTexture(*(Texture::GetTextureByName(DEFAULT_TEXTURE_NAME)));
   SetLineWidth(lineThickness);

   Vertex_PCT vertexes[2];
   vertexes[0] = { startPos, startColor, TexCoords::ZERO };
   vertexes[1] = { endPos, endColor, TexCoords::ZERO };
   DrawVertexArray_PCT(vertexes, 2, PRIMITIVE_LINES);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawLine(const Vector3f& startPos, const Vector3f& endPos, const Rgba& color, float lineThickness /*= 1.f*/) const
{
   BindTexture(*(Texture::GetTextureByName(DEFAULT_TEXTURE_NAME)));
   SetLineWidth(lineThickness);

   Vertex_PCT vertexes[2];
   vertexes[0] = { startPos, color, TexCoords::ZERO };
   vertexes[1] = { endPos, color, TexCoords::ZERO };
   DrawVertexArray_PCT(vertexes, 2, PRIMITIVE_LINES);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawAABB2(const AABB2& bounds, const Rgba& color, const Texture* texture, TexBounds texBounds /*= 0,0 to 1,1*/) const
{
   if (texture == nullptr)
   {
      texture = Texture::GetTextureByName(DEFAULT_TEXTURE_NAME);
   }

   BindTexture(*texture);
   SetLineWidth(1.f);

   Vertex_PCT vertsToDraw[4];
   vertsToDraw[0] = { Vector3f(bounds.mins.x, bounds.mins.y, 0.f), color, TexCoords(texBounds.mins.x, texBounds.maxs.y) };
   vertsToDraw[1] = { Vector3f(bounds.maxs.x, bounds.mins.y, 0.f), color, TexCoords(texBounds.maxs.x, texBounds.maxs.y) };
   vertsToDraw[2] = { Vector3f(bounds.maxs.x, bounds.maxs.y, 0.f), color, TexCoords(texBounds.maxs.x, texBounds.mins.y) };
   vertsToDraw[3] = { Vector3f(bounds.mins.x, bounds.maxs.y, 0.f), color, TexCoords(texBounds.mins.x, texBounds.mins.y) };
   DrawVertexArray_PCT(vertsToDraw, 4, PRIMITIVE_QUADS);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawQuad(const Vector2f& botLeft, const Vector2f& botRight, const Vector2f& topLeft, const Vector2f& topRight, const Rgba& color, const Texture* texture, TexBounds texBounds) const
{
   if (texture == nullptr)
   {
      texture = Texture::GetTextureByName(DEFAULT_TEXTURE_NAME);
   }

   BindTexture(*texture);
   SetLineWidth(1.f);

   const float width = botRight.x - botLeft.x;
   const float height = topLeft.y - topRight.y;

   Vertex_PCT vertsToDraw[4];
   vertsToDraw[0] = { Vector3f(botLeft.x, botLeft.y, 0.f), color, TexCoords(texBounds.mins.x, texBounds.maxs.y) };
   vertsToDraw[1] = { Vector3f(botRight.x, botRight.y, 0.f), color, TexCoords(texBounds.maxs.x, texBounds.maxs.y) };
   vertsToDraw[2] = { Vector3f(topRight.x, topRight.y, 0.f), color, TexCoords(texBounds.maxs.x, texBounds.mins.y) };
   vertsToDraw[3] = { Vector3f(topLeft.x, topLeft.y, 0.f), color, TexCoords(texBounds.mins.x, texBounds.mins.y) };
   DrawVertexArray_PCT(vertsToDraw, 4, PRIMITIVE_QUADS);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::ClearScreen(const Rgba& backgroundColor) const
{
   glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
   glClear(GL_COLOR_BUFFER_BIT);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::ClearScreen(const Rgba& backgroundColor, float depthClear) const
{
   float r = RangeMap((float)backgroundColor.r, 0.f, 255.f, 0.f, 1.f);
   float g = RangeMap((float)backgroundColor.g, 0.f, 255.f, 0.f, 1.f);
   float b = RangeMap((float)backgroundColor.b, 0.f, 255.f, 0.f, 1.f);
   float a = RangeMap((float)backgroundColor.a, 0.f, 255.f, 0.f, 1.f);
   glClearColor(r, g, b, a);
   glClearDepth(depthClear);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}


//-----------------------------------------------------------------------------------------------
void TheRenderer::EnableRenderCapability(int capability) const
{
   glEnable(capability);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::UseMaterial(const Material* mat) const
{
   GPUProgram program = mat->GetProgram();

   glUseProgram(program.GetID());
   BindRenderState(mat->GetRenderState());

   std::map<std::string, const Texture*> textures = mat->GetTextures();
   int texIndex = 0;
   for (auto texIter = textures.begin(); texIter != textures.end(); ++texIter)
   {
      glBindSampler(texIndex, mat->GetSamplerID());
      glActiveTexture(GL_TEXTURE0 + texIndex);
      glBindTexture(GL_TEXTURE_2D, texIter->second->GetTextureID());
      const UniformData* textureUniform = mat->GetUniformByName(texIter->first);

      glUniform1i(textureUniform->bindPos, texIndex);
      ++texIndex;
   }

   BindMaterialUniforms(mat);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::BindRenderState(const RenderState& state) const
{
   SetCullMode(state.cullMode);
   SetBlendFunc(state.sourceBlend, state.destBlend);
   SetFillMode(state.fillMode);
   SetWindingOrder(state.windingOrder);
   SetDepthFunc(state.depthFunc);

   if (state.isUsingDepthTest)
   {
      EnableDepthTesting();
   }
   else
   {
      DisableDepthTesting();
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::SetBlendFunc(BlendFactor source, BlendFactor dest) const
{
   GLenum srcEnum = GetEnumForBlendFactor(source);
   GLenum destEnum = GetEnumForBlendFactor(dest);

   glBlendFunc(srcEnum, destEnum);
}


//-----------------------------------------------------------------------------------------------
GLenum TheRenderer::GetEnumForBlendFactor(BlendFactor factor) const
{
   switch (factor)
   {
   case ZERO_BLEND:
   {
      return GL_ZERO;
   }

   case FULL_BLEND:
   {
      return GL_ONE;
   }

   case SOURCE_COLOR:
   {
      return GL_SRC_COLOR;
   }

   case INVERT_SOURCE_COLOR:
   {
      return GL_ONE_MINUS_SRC_COLOR;
   }

   case SOURCE_ALPHA:
   {
      return GL_SRC_ALPHA;
   }

   case INVERT_SOURCE_ALPHA:
   {
      return GL_ONE_MINUS_SRC_ALPHA;
   }

   case DEST_COLOR:
   {
      return GL_DST_COLOR;
   }

   case INVERT_DEST_COLOR:
   {
      return GL_ONE_MINUS_DST_COLOR;
   }

   case DEST_ALPHA:
   {
      return GL_DST_ALPHA;
   }

   case INVERT_DEST_ALPHA:
   {
      return GL_ONE_MINUS_DST_ALPHA;
   }
   }

   // Option not supported; return black
   return GL_ZERO;
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::SetCullMode(CullMode mode) const
{
   switch (mode)
   {
   case CULL_NONE:
   {
      glDisable(GL_CULL_FACE);
      return;
   }
   case CULL_FRONT:
   {
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);
      return;
   }
   case CULL_BACK:
   {
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      return;
   }
   case CULL_ALL:
   {
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT_AND_BACK);
      return;
   }
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::SetFillMode(FillMode mode) const
{
   switch (mode)
   {
   case FILL_POINT:
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
      return;
   }

   case FILL_LINE:
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      return;
   }

   case FILL_FULL:
   {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      return;
   }
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::SetWindingOrder(WindingOrder order) const
{
   switch (order)
   {
   case WIND_CCW:
   {
      glFrontFace(GL_CCW);
      return;
   }
   case WIND_CW:
   {
      glFrontFace(GL_CW);
      return;
   }
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::SetDepthFunc(DepthFunc func) const
{
   switch (func)
   {
   case WRITE_NEVER:
   {
      glDepthFunc(GL_NEVER);
      return;
   }

   case WRITE_LESS:
   {
      glDepthFunc(GL_LESS);
      return;
   }

   case WRITE_LESS_OR_EQUAL:
   {
      glDepthFunc(GL_LEQUAL);
      return;
   }

   case WRITE_EQUAL:
   {
      glDepthFunc(GL_EQUAL);
      return;
   }

   case WRITE_GREATER:
   {
      glDepthFunc(GL_GREATER);
      return;
   }

   case WRITE_NOT_EQUAL:
   {
      glDepthFunc(GL_NOTEQUAL);
      return;
   }

   case WRITE_GREATER_OR_EQUAL:
   {
      glDepthFunc(GL_GEQUAL);
      return;
   }

   case WRITE_ALWAYS:
   {
      glDepthFunc(GL_ALWAYS);
      return;
   }
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::EnableDepthTesting() const
{
    glEnable(GL_DEPTH_TEST);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DisableDepthTesting() const
{
    glDisable(GL_DEPTH_TEST);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::EnableBackFaceCulling() const
{
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DisableCulling() const
{
   glDisable(GL_CULL_FACE);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::EnableWireFrameFilling() const
{
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


// #TODO - add Create mesh that takes a byte buffer for the verts and a vert def
//-----------------------------------------------------------------------------------------------
Mesh* TheRenderer::CreateMesh(const std::vector<Vertex_PCT>& verts, const std::vector<GLuint>& indices) const
{
   GLuint vboID = RenderBufferCreate((void*)&verts[0], verts.size(), sizeof(verts[0]), STATIC_DRAW);
   GLuint iboID = RenderBufferCreate((void*)&indices[0], indices.size(), sizeof(indices[0]), STATIC_DRAW);
   Mesh* newMesh = new Mesh(vboID, iboID, &m_vertexPCT);
   return newMesh;
}


//-----------------------------------------------------------------------------------------------
Mesh* TheRenderer::CreateMesh(const std::vector<Vertex_PCUTB>& verts, const std::vector<GLuint>& indices) const
{
   GLuint vboID = RenderBufferCreate((void*)&verts[0], verts.size(), sizeof(verts[0]), STATIC_DRAW);
   GLuint iboID = RenderBufferCreate((void*)&indices[0], indices.size(), sizeof(indices[0]), STATIC_DRAW);
   Mesh* newMesh = new Mesh(vboID, iboID, &m_vertexPCUTB);
   return newMesh;
}


//-----------------------------------------------------------------------------------------------
MeshRenderer* TheRenderer::CreateMeshRenderer(Mesh* mesh, Material* mat) const
{
   GLuint newVAO;
   glGenVertexArrays(1, &newVAO);
   BindMeshToVAO(newVAO, mesh->GetVBO(), mesh->GetIBO(), mat->GetProgram());
   MeshRenderer* newMeshRenderer = new MeshRenderer(newVAO, mesh, mat);

   return newMeshRenderer;
}

//-----------------------------------------------------------------------------------------------
MeshRenderer* TheRenderer::CreateMeshRenderer(Mesh* mesh) const
{
   GLuint newVAO;
   glGenVertexArrays(1, &newVAO);
   BindMeshToVAO(newVAO, mesh->GetVBO(), mesh->GetIBO(), m_defaultMaterial->GetProgram());
   MeshRenderer* newMeshRenderer = new MeshRenderer(newVAO, mesh, m_defaultMaterial);

   return newMeshRenderer;
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::SetLineWidth(float width) const
{
   glLineWidth(width);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::SetPointSize(float size) const
{
   glPointSize(size);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawOriginAxis(float length, float lineThickness, unsigned char alpha, bool isDepthTesting) const
{
   Vector3f origin(0.f, 0.f, 0.f);
   Vector3f xDir(length, 0.f, 0.f);
   Vector3f yDir(0.f, length, 0.f);
   Vector3f zDir(0.f, 0.f, length);

   if (isDepthTesting)
   {
      EnableDepthTesting();
   }
   else
   {
      DisableDepthTesting();
   }

   DrawLine(origin, xDir, Rgba(255, 0, 0, alpha), lineThickness);
   DrawLine(origin, yDir, Rgba(0, 255, 0, alpha), lineThickness);
   DrawLine(origin, zDir, Rgba(0, 0, 255, alpha), lineThickness);
}


//-----------------------------------------------------------------------------------------------
// 2D only; assumes max width with rightDir = 1.f, 0.f, 0.f
float TheRenderer::CalcTextWidth(const std::string& text, float cellHeight, const BitmapFont* font) const
{
   if (font == nullptr)
   {
      font = BitmapFont::CreateOrGetFont(DEFAULT_FONT_NAME);
   }

   if (font == nullptr)
   {
      ERROR_AND_DIE("Default font failed to load.");
   }

   float scale = cellHeight / font->GetFontSize();
   const Glyph* prevGlyph = nullptr;

   float totalWidth = 0.f;
   for (unsigned stringIndex = 0; stringIndex < text.length(); ++stringIndex)
   {
      const Glyph* currentGlyph = font->GetGlyphForID(text[stringIndex]);
      if (currentGlyph == nullptr)
      {
         continue;
      }

      // Glyph data
      float offsetX = currentGlyph->xOffset;
      float width = currentGlyph->width;

      //Kerning
      float kerningAmount = (float)font->GetKerningForPair(prevGlyph, currentGlyph);

      totalWidth += scale * (kerningAmount + offsetX + width);
      totalWidth += scale * (currentGlyph->xAdvance - offsetX - width);
      prevGlyph = currentGlyph;
   }

   return totalWidth;
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawText2D(const Vector2f& startPositionTopLeft, const std::string& content, const Rgba& color, float cellHeight, const BitmapFont* font, const Vector2f& upDir, const Vector2f& rightDir) const
{
   if (font == nullptr)
   {
      font = BitmapFont::CreateOrGetFont(DEFAULT_FONT_NAME);
   }

   if (font == nullptr)
   {
      ERROR_AND_DIE("Default font failed to load.");
   }

   Vector2f cursorPosition = startPositionTopLeft;
   Vector2f textureDimensions = font->GetTextureDimensions();
   float scale = cellHeight / font->GetFontSize();

   const Glyph* prevGlyph = nullptr;

   for (unsigned stringIndex = 0; stringIndex < content.length(); ++stringIndex)
   {
      const Glyph* currentGlyph = font->GetGlyphForID(content[stringIndex]);
      if (currentGlyph == nullptr)
      {
         continue;
      }

      // Glyph data
      Vector2f offset((float)currentGlyph->xOffset, (float)-currentGlyph->yOffset);
      Vector2f size(currentGlyph->width, currentGlyph->height);
      Vector2f texturePixel(currentGlyph->xStart, currentGlyph->yStart);

      //Kerning
      char kerningAmount = font->GetKerningForPair(prevGlyph, currentGlyph);
      cursorPosition += rightDir * scale * kerningAmount;

      // Screen Position
      Vector2f topLeft = cursorPosition
         + (rightDir * offset.x * scale)
         + (upDir * offset.y * scale);
      topLeft.y = ceil(topLeft.y);
      topLeft.x = floor(topLeft.x);

      Vector2f topRight = topLeft
         + (rightDir * size.x * scale);
      topRight.y = ceil(topRight.y);
      topRight.x = ceil(topRight.x);

      Vector2f botLeft = topLeft
         - (upDir * size.y * scale);
      botLeft.y = floor(botLeft.y);
      botLeft.x = floor(botLeft.x);

      Vector2f botRight = botLeft
         + (rightDir * size.x * scale);
      botRight.y = floor(botRight.y);
      botRight.x = ceil(botRight.x);

      // Texture Position
      TexCoords botLeftTexCoord(texturePixel.x / textureDimensions.x,
         texturePixel.y / textureDimensions.y);

      TexCoords botRightTexCoord = botLeftTexCoord;
      botRightTexCoord.x += (size.x / textureDimensions.x);

      TexCoords topLeftTexCoord = botLeftTexCoord;
      topLeftTexCoord.y += (size.y / textureDimensions.y);

      TexCoords topRightTexCoord = topLeftTexCoord;
      topRightTexCoord.x += (size.x / textureDimensions.x);

      DrawQuad(botLeft, botRight, topLeft, topRight, color, font->GetTexture(), TexBounds(botLeftTexCoord, topRightTexCoord));

      // Move to next position
      // Note: yAdvance not supported
      cursorPosition += (rightDir * scale * currentGlyph->xAdvance);

      prevGlyph = currentGlyph;
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawText3D(const Vector3f& startPositionTopLeft, const std::string& content, const Rgba& color, float cellHeight, const BitmapFont* font, const Vector3f& upDir, const Vector3f& rightDir) const

{
   if (font == nullptr)
   {
      font = BitmapFont::CreateOrGetFont(DEFAULT_FONT_NAME);
   }

   if (font == nullptr)
   {
      ERROR_AND_DIE("Default font failed to load.");
   }

   Vector3f cursorPosition = startPositionTopLeft;
   Vector2f textureDimensions = font->GetTextureDimensions();
   const float scale = cellHeight / font->GetFontSize();

   const Glyph* prevGlyph = nullptr;

   for (unsigned stringIndex = 0; stringIndex < content.length(); ++stringIndex)
   {
      const Glyph* currentGlyph = font->GetGlyphForID(content[stringIndex]);
      if (currentGlyph == nullptr)
      {
         continue;
      }

      // Glyph data
      Vector2f offset((float)currentGlyph->xOffset, (float)-currentGlyph->yOffset);
      Vector2f size(currentGlyph->width, currentGlyph->height);
      Vector2f texturePixel(currentGlyph->xStart, currentGlyph->yStart);

      //Kerning
      const char kerningAmount = font->GetKerningForPair(prevGlyph, currentGlyph);
      cursorPosition += rightDir * scale * kerningAmount;

      // Screen Position
      Vector3f topLeft = cursorPosition
         + (rightDir * offset.x * scale)
         + (upDir * offset.y * scale);

      Vector3f topRight = topLeft
         + (rightDir * size.x * scale);

      Vector3f botLeft = topLeft
         - (upDir * size.y * scale);

      Vector3f botRight = botLeft
         + (rightDir * size.x * scale);

      // Texture Position
      Vector2f botLeftTexCoord(texturePixel.x / textureDimensions.x,
         texturePixel.y / textureDimensions.y);

      Vector2f botRightTexCoord = botLeftTexCoord;
      botRightTexCoord.x += (size.x / textureDimensions.x);

      Vector2f topLeftTexCoord = botLeftTexCoord;
      topLeftTexCoord.y += (size.y / textureDimensions.y);

      Vector2f topRightTexCoord = topLeftTexCoord;
      topRightTexCoord.x += (size.x / textureDimensions.x);

      Draw3DQuad(botLeft, botRight, topRight, topLeft, color, font->GetTexture(), TexBounds(botLeftTexCoord, topRightTexCoord));

      // Move to next position
      // Note: yAdvance not supported
      cursorPosition += (rightDir * scale * currentGlyph->xAdvance);

      prevGlyph = currentGlyph;
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::Draw3DQuad(const Vector3f& botLeft, const Vector3f& botRight, const Vector3f& topRight, const Vector3f& topLeft, const Rgba& tint, const Texture* texture, TexBounds texBounds) const
{
   if (texture == nullptr)
   {
      texture = Texture::GetTextureByName(DEFAULT_TEXTURE_NAME);
   }

   BindTexture(*texture);

   Vertex_PCT quadVertexes[4];
   quadVertexes[0] = { botLeft, tint, TexCoords(texBounds.mins.x, texBounds.maxs.y) };
   quadVertexes[1] = { botRight, tint, TexCoords(texBounds.maxs.x, texBounds.maxs.y) };
   quadVertexes[2] = { topRight, tint, TexCoords(texBounds.maxs.x, texBounds.mins.y) };
   quadVertexes[3] = { topLeft, tint, TexCoords(texBounds.mins.x, texBounds.mins.y) };
   DrawVertexArray_PCT(quadVertexes, 4, PRIMITIVE_QUADS);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::EnableContrastBlending() const
{
   glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::BindTexture(const Texture& texture) const
{
   //glEnable(GL_TEXTURE_2D);
   //glBindTexture(GL_TEXTURE_2D, texture.GetTextureID());
   
   glBindSampler(0, NULL);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture.GetTextureID());
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawHollowQuad(const Vector2f& botLeft, const Vector2f& botRight, const Vector2f& topRight, const Vector2f& topLeft, const Rgba& tint, float thickness) const
{
   BindTexture(*(Texture::GetTextureByName(DEFAULT_TEXTURE_NAME)));
   SetLineWidth(thickness);

   Vertex_PCT squareVertexes[4];
   squareVertexes[0] = { Vector3f(botLeft.x, botLeft.y, 0.f), tint, TexCoords::ZERO };
   squareVertexes[1] = { Vector3f(botRight.x, botRight.y, 0.f), tint, TexCoords::ZERO };
   squareVertexes[2] = { Vector3f(topRight.x, topRight.y, 0.f), tint, TexCoords::ZERO };
   squareVertexes[3] = { Vector3f(topLeft.x, topLeft.y, 0.f), tint, TexCoords::ZERO };
   DrawVertexArray_PCT(squareVertexes, 4, PRIMITIVE_LINE_LOOP);
}

//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawHollowQuad(const Vector3f& botLeft, const Vector3f& botRight, const Vector3f& topRight, const Vector3f& topLeft, const Rgba& tint, float thickness) const
{
   BindTexture(*(Texture::GetTextureByName(DEFAULT_TEXTURE_NAME)));
   SetLineWidth(thickness);

   Vertex_PCT squareVertexes[4];
   squareVertexes[0] = { botLeft, tint, TexCoords::ZERO };
   squareVertexes[1] = { botRight, tint, TexCoords::ZERO };
   squareVertexes[2] = { topRight, tint, TexCoords::ZERO };
   squareVertexes[3] = { topLeft, tint, TexCoords::ZERO };
   DrawVertexArray_PCT(squareVertexes, 4, PRIMITIVE_LINE_LOOP);
}


//TODO: Only draw around z-axis atm. NEED TO FIX
//-----------------------------------------------------------------------------------------------
//void TheRenderer::DrawHollowCircle(const Vector3f& centerPos, float radius, const Vector3f& axis, const Rgba& tint, int numSides) const
//{
//   const float radiansPerSide = TAU / (float)numSides;
//   int numVertices = (int)((360.f / radiansPerSide) + 1);
//
//   std::vector<Vertex_PCT> vertexes;
//   vertexes.reserve(numVertices);
//   
//   for (float radians = 0.f; radians < TAU; radians += radiansPerSide)
//   {
//      float x = radius * cos(radians);
//      float y = radius * sin(radians);
//
//      vertexes.push_back(Vertex_PCT(centerPos + Vector3f(x, y, 0.f), tint));
//   }
//
//   DrawVertexArray_PCT(&vertexes[0], vertexes.size(), PRIMITIVE_LINE_LOOP);
//}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawVertexArray_PCT(const Vertex_PCT* const vertexes, int numVertexes, PrimitiveType primitiveType) const
{
   if (numVertexes == 0)
   {
      return;
   }

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glVertexPointer(3,   GL_FLOAT,         sizeof(Vertex_PCT), &vertexes[0].position);
   glColorPointer(4,    GL_UNSIGNED_BYTE, sizeof(Vertex_PCT), &vertexes[0].color);
   glTexCoordPointer(2, GL_FLOAT,         sizeof(Vertex_PCT), &vertexes[0].texCoords);

   switch (primitiveType)
   {
   case PRIMITIVE_POINTS:
   {
      glDrawArrays(GL_POINTS, 0, numVertexes);
      break;
   }
   case PRIMITIVE_LINES:
   {
      glDrawArrays(GL_LINES, 0, numVertexes);
      break;
   }
   case PRIMITIVE_LINE_LOOP:
   {
      glDrawArrays(GL_LINE_LOOP, 0, numVertexes);
      break;
   }
   case PRIMITIVE_TRIANGLES:
   {
      glDrawArrays(GL_TRIANGLES, 0, numVertexes);
      break;
   }
   case PRIMITIVE_TRIANGLE_STRIPS:
   {
      glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertexes);
   }
   case PRIMITIVE_QUADS:
   {
      glDrawArrays(GL_QUADS, 0, numVertexes);
      break;
   }
   }

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawVBO_PCT(unsigned int vboID, unsigned int numVertexes, PrimitiveType primitiveType) const
{
   BindVBO(vboID);

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glVertexPointer(3,   GL_FLOAT,         sizeof(Vertex_PCT), (const GLvoid*) offsetof(Vertex_PCT, position));
   glColorPointer(4,    GL_UNSIGNED_BYTE, sizeof(Vertex_PCT), (const GLvoid*) offsetof(Vertex_PCT, color));
   glTexCoordPointer(2, GL_FLOAT,         sizeof(Vertex_PCT), (const GLvoid*) offsetof(Vertex_PCT, texCoords));

   switch (primitiveType)
   {
   case PRIMITIVE_POINTS:
   {
      glDrawArrays(GL_POINTS, 0, numVertexes);
      break;
   }
   case PRIMITIVE_LINES:
   {
      glDrawArrays(GL_LINES, 0, numVertexes);
      break;
   }
   case PRIMITIVE_LINE_LOOP:
   {
      glDrawArrays(GL_LINE_LOOP, 0, numVertexes);
      break;
   }
   case PRIMITIVE_TRIANGLES:
   {
      glDrawArrays(GL_TRIANGLES, 0, numVertexes);
      break;
   }
   case PRIMITIVE_QUADS:
   {
      glDrawArrays(GL_QUADS, 0, numVertexes);
      break;
   }
   default:
   {
   }
   }

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   BindVBO(UNBIND_BUFFER_ID);
}


//-----------------------------------------------------------------------------------------------
Shader TheRenderer::LoadShader(const std::string& shaderName, GLuint shaderType) const
{
   std::string fileName = Stringf("Data/Shaders/%s", shaderName.c_str());

   std::vector<unsigned char> fileBuffer;
   ASSERT_OR_DIE(LoadBinaryFileToBuffer(fileName, fileBuffer), Stringf("File %s not found!", fileName));;
   fileBuffer.push_back(0);

   const char* shaderBuffer = (const char*)fileBuffer.data();

   GLuint shaderID = glCreateShader(shaderType);
   ASSERT_OR_DIE(shaderID != NULL, Stringf("Failed to load shader %s", fileName.c_str())); //or 0

   GLint sourceLength = strlen(shaderBuffer);
   glShaderSource(shaderID, 1, &shaderBuffer, &sourceLength);

   glCompileShader(shaderID);

   // Check for errors
   GLint status;
   glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
   if (status == GL_FALSE)
   {
      LogShaderError(fileName, shaderID, false);
      shaderID = NULL;
   }

   Shader newShader(shaderID, shaderName);
   return newShader;
}


//-----------------------------------------------------------------------------------------------
GPUProgram TheRenderer::CreateAndLinkProgram(const Shader& vertShader, const Shader& fragShader) const
{
   GLuint programID = glCreateProgram();
   ASSERT_OR_DIE(programID != NULL, "Failed to create program.");

   glAttachShader(programID, vertShader.GetID());
   glAttachShader(programID, fragShader.GetID());

   glLinkProgram(programID);

   GLint status;
   glGetProgramiv(programID, GL_LINK_STATUS, &status);
   if (status == GL_FALSE)
   {
      LogShaderError(fragShader.GetFileName(), programID, true);
      programID = NULL;
   }
   else
   {
      glDetachShader(programID, vertShader.GetID());
      glDetachShader(programID, fragShader.GetID());
   }

   GPUProgram defaultShaderProgram(programID);

   return defaultShaderProgram;
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DeleteProgram(const GPUProgram& program) const
{
   glDeleteProgram(program.GetID());
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::LogShaderError(const std::string& filePath, GLuint id, bool isProgram) const
{
   GLint logLength;

   if (isProgram)
   {
      glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
   }
   else
   {
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
   }

   std::string logInfo;
   logInfo.resize(logLength);

   if (isProgram)
   {
      glGetProgramInfoLog(id, logLength, &logLength, &logInfo[0]);
   }
   else
   {
      glGetShaderInfoLog(id, logLength, &logLength, &logInfo[0]);
   }
   
   std::stringstream infoStream(logInfo);
   int temp;
   infoStream >> temp;
   std::string errorMessage = _fullpath(NULL, filePath.c_str(), _MAX_PATH);
   while (!infoStream.eof())
   {
      std::string tempStr;
      infoStream >> tempStr;
      errorMessage.append(tempStr);
   } // TODO: This can apparently break

   DebuggerPrintf("%s\n", errorMessage.c_str());
   MakeShaderErrorWindow(filePath, logInfo);

   if (isProgram)
   {
      glDeleteProgram(id);
   }
   else
   {
      glDeleteShader(id);
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::MakeShaderErrorWindow(const std::string& filePath, const std::string& errorLog) const
{
   std::stringstream errorStream(errorLog.c_str());
   int lineNumber;
   errorStream >> lineNumber;
   errorStream.ignore(1, '(');
   errorStream >> lineNumber;
   errorStream.ignore(256, ':');
   errorStream.ignore(256, ':');

   std::string windowMessage = Stringf("Error on line %d in file %s:\n\n", lineNumber, GetFullPath(filePath));
   while (!errorStream.eof())
   {
      std::string temp;
      errorStream >> temp;
      windowMessage += temp + ' ';
   }
   windowMessage += "\n\n";
   windowMessage += GetOpenGLVersion();
   windowMessage += GetGLSLVersion();

   MessageBoxA(NULL, windowMessage.c_str(), "Shader Error", MB_OK);
}


//-----------------------------------------------------------------------------------------------
std::string TheRenderer::GetOpenGLVersion() const
{
   return Stringf("OpenGL version is: %s\n", glGetString(GL_VERSION));
}


//-----------------------------------------------------------------------------------------------
std::string TheRenderer::GetGLSLVersion() const
{
   return Stringf("GLSL version is: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawMeshRenderer(const MeshRenderer* meshRenderer) const
{
   glBindVertexArray(meshRenderer->GetVAO());
   
   const Material* mat = meshRenderer->GetMaterial();
   UseMaterial(mat);

   //g_theRenderer->EnableWireFrameFilling();// #TODO - use bool in function call
   const Mesh* mesh = meshRenderer->GetMesh();
   DrawMesh(mesh);

   // cleanup
   glUseProgram(NULL);
   glBindVertexArray(NULL);
   glBindSampler(0, NULL);
   glBindTexture(GL_TEXTURE_2D, NULL);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DrawMesh(const Mesh* mesh) const
{
   const std::vector<DrawInstruction>& instructions = mesh->GetDrawInstructions();

   for (const DrawInstruction& instruction : instructions)
   {
      // #TODO - always triangle for now
      //Glenum primitive = GetPrimitiveForEnum(instruction.primitiveType);
      GLenum primitive = GL_TRIANGLES;
      GLint startIndex = instruction.startIndex; // #DEBUG
      int count = instruction.count;
      
      if (instruction.isUsingIndexBuffer)
      {
         glDrawElements(primitive, count, GL_UNSIGNED_INT, (GLvoid*)(startIndex * sizeof(int)));
      }
      else // !instruction.isUsingIndexBuffer
      {
         glDrawArrays(primitive, startIndex, count);
      }
   }
}


//-----------------------------------------------------------------------------------------------
Mesh* TheRenderer::GenerateSphereMeshPCT(float radius, int slices, int stacks) const
{
   std::vector<Vertex_PCT> verts;
   std::vector<GLuint> indices;

   const float diameter = radius * 2.f;

   const float stackAngleRadians = TAU / (float)stacks;
   const float sliceAngleRadians = TAU / (float)slices;

   // verts
   for (int stackIndex = 0; stackIndex < stacks; ++stackIndex)
   {
      float currentHeight = radius - ((diameter / stacks) * stackIndex);

      float temp = RangeMap((float)stackIndex, 0.f, (float)stacks - 1.f, -1.f, 1.f);
      float currentWidth = sqrt(1.f - (temp * temp)) * radius;

      for (int sliceIndex = 0; sliceIndex < slices; ++sliceIndex)
      {
         float thisSliceAngle = sliceAngleRadians * sliceIndex;
         float x = currentWidth * cos(thisSliceAngle);
         float z = currentWidth * sin(thisSliceAngle);
         Vector3f newPoint(x, currentHeight, z);
         verts.push_back(Vertex_PCT(newPoint, Rgba::WHITE, TexCoords(x, (float)stackIndex / (float)(stacks - 1))));
      }
   }

   // indices
   for (int stackIndex = 0; stackIndex < stacks; ++stackIndex)
   {
      for (int sliceIndex = 0; sliceIndex < slices; ++sliceIndex)
      {
         GLuint topLeft = sliceIndex + (stackIndex * slices);
         GLuint topRight = topLeft + 1;
         if (topRight == (GLuint)(stackIndex * slices) + slices)
         {
            topRight -= slices;
         }

         GLuint botLeft = topLeft + slices;
         GLuint botRight = topRight + slices;

         indices.push_back(botLeft);
         indices.push_back(topRight);
         indices.push_back(topLeft);

         indices.push_back(botLeft);
         indices.push_back(botRight);
         indices.push_back(topRight);
      }
   }

   Mesh* sphereMesh = CreateMesh(verts, indices);
   return sphereMesh;
}


// #TODO - move all this into a builder
//-----------------------------------------------------------------------------------------------
Mesh* TheRenderer::GenerateSphereMeshPCUTB(float radius, int slices, int stacks) const
{
   std::vector<Vertex_PCUTB> verts;
   std::vector<GLuint> indices;

   const float thetaIncrement = TAU / (float)slices;
   const float psiIncrement = PI / (float)stacks;

   MeshBuilder builder;
   builder.BeginDrawInstruction();
   // verts
   for (float psiAngleRadians = -PI / 2.f; psiAngleRadians < PI / 2.f + psiIncrement; psiAngleRadians += psiIncrement)
   {
      for (float thetaAngleRadians = 0; thetaAngleRadians < TAU; thetaAngleRadians += thetaIncrement)
      {
         float z = radius * sin(thetaAngleRadians) * cos(psiAngleRadians);
         float y = radius * sin(psiAngleRadians);
         float x = radius * cos(thetaAngleRadians) * cos(psiAngleRadians);
         Vector3f newPoint(x, y, z);

         float u = thetaAngleRadians / TAU;
         float v = 1.f - (psiAngleRadians + (PI / 2.f)) / PI;
         TexCoords newUV(u, v);

         float tanZ = radius * cos(thetaAngleRadians) * cos(psiAngleRadians);
         float tanX = radius * -sin(thetaAngleRadians) * cos(psiAngleRadians);
         Vector3f newTan(tanX, 0.f, tanZ);

         float bitZ = radius * -sin(thetaAngleRadians) * sin(psiAngleRadians);
         float bitY = radius * cos(psiAngleRadians);
         float bitX = radius * -cos(thetaAngleRadians) * sin(psiAngleRadians);
         Vector3f newBit(bitX, bitY, bitZ);

         builder.SetCurrentVertexPosition(newPoint);
         builder.SetCurrentVertexUV0(newUV);
         builder.SetCurrentVertexTangent(newTan);
         builder.SetCurrentVertexBitangent(newBit);
         builder.AddCurrentVertexToMesh();
      }
   }

   // indices
   for (int psiIndex = 0; psiIndex <= stacks + 1; ++psiIndex)
   {
      for (int thetaIndex = 0; thetaIndex < slices; ++thetaIndex)
      {
         GLuint botLeft = thetaIndex + (psiIndex * slices);
         GLuint botRight = botLeft + 1;
         if (botRight == (GLuint)((psiIndex + 1) * slices) + 1)
         {
            botRight -= slices;
         }

         GLuint topLeft = botLeft + slices;
         GLuint topRight = botRight + slices;

         builder.AddTriangleIndices(botLeft, botRight, topRight);
         builder.AddTriangleIndices(botLeft, topRight, topLeft);
      }
   }
   builder.EndDrawInstruction();

   GLuint vboID = GenerateVBO();
   GLuint iboID = GenerateVBO();
   Mesh* mesh = new Mesh(vboID, iboID, &m_vertexPCUTB);
   builder.CopyToMesh(mesh, CopyToPCUTB);
   return mesh;
}


//-----------------------------------------------------------------------------------------------
Mesh* TheRenderer::GenerateCubeMeshPCT(float length, float width, float height) const
{
   const float halfLength = length / 2.f;
   const float halfWidth = width / 2.f;
   const float halfHeight = height / 2.f;

   std::vector<Vertex_PCT> verts;
   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, -halfHeight, -halfLength), Rgba::WHITE, TexCoords(0.f, 1.f)));
   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, halfHeight, -halfLength), Rgba::WHITE, TexCoords(0.f, 0.f)));
   verts.push_back(Vertex_PCT(Vector3f(halfWidth, halfHeight, -halfLength), Rgba::WHITE, TexCoords(1.f, 0.f)));
   verts.push_back(Vertex_PCT(Vector3f(halfWidth, -halfHeight, -halfLength), Rgba::WHITE, TexCoords(1.f, 1.f)));

   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, -halfHeight, halfLength), Rgba::WHITE, TexCoords(1.f, 1.f)));
   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, halfHeight, halfLength), Rgba::WHITE, TexCoords(1.f, 0.f)));
   verts.push_back(Vertex_PCT(Vector3f(halfWidth, halfHeight, halfLength), Rgba::WHITE, TexCoords(0.f, 0.f)));
   verts.push_back(Vertex_PCT(Vector3f(halfWidth, -halfHeight, halfLength), Rgba::WHITE, TexCoords(0.f, 1.f)));

   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, halfHeight, -halfLength), Rgba::WHITE, TexCoords(0.f, 1.f)));
   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, halfHeight, halfLength), Rgba::WHITE, TexCoords(0.f, 0.f)));
   verts.push_back(Vertex_PCT(Vector3f(halfWidth, halfHeight, halfLength), Rgba::WHITE, TexCoords(1.f, 0.f)));
   verts.push_back(Vertex_PCT(Vector3f(halfWidth, halfHeight, -halfLength), Rgba::WHITE, TexCoords(1.f, 1.f)));

   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, -halfHeight, halfLength), Rgba::WHITE, TexCoords(0.f, 1.f)));
   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, -halfHeight, -halfLength), Rgba::WHITE, TexCoords(0.f, 0.f)));
   verts.push_back(Vertex_PCT(Vector3f(halfWidth, -halfHeight, -halfLength), Rgba::WHITE, TexCoords(1.f, 0.f)));
   verts.push_back(Vertex_PCT(Vector3f(halfWidth, -halfHeight, halfLength), Rgba::WHITE, TexCoords(1.f, 1.f)));

   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, -halfHeight, halfLength), Rgba::WHITE, TexCoords(0.f, 1.f)));
   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, -halfHeight, -halfLength), Rgba::WHITE, TexCoords(1.f, 1.f)));
   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, halfHeight, -halfLength), Rgba::WHITE, TexCoords(1.f, 0.f)));
   verts.push_back(Vertex_PCT(Vector3f(-halfWidth, halfHeight, halfLength), Rgba::WHITE, TexCoords(0.f, 0.f)));

   verts.push_back(Vertex_PCT(Vector3f(halfWidth, -halfHeight, -halfLength), Rgba::WHITE, TexCoords(0.f, 1.f)));
   verts.push_back(Vertex_PCT(Vector3f(halfWidth, halfHeight, halfLength), Rgba::WHITE, TexCoords(1.f, 0.f)));
   verts.push_back(Vertex_PCT(Vector3f(halfWidth, halfHeight, -halfLength), Rgba::WHITE, TexCoords(0.f, 0.f)));
   verts.push_back(Vertex_PCT(Vector3f(halfWidth, -halfHeight, halfLength), Rgba::WHITE, TexCoords(1.f, 1.f)));


   std::vector<GLuint> indices;
   //front
   indices.push_back(0);
   indices.push_back(2);
   indices.push_back(1);
   indices.push_back(0);
   indices.push_back(3);
   indices.push_back(2);

   //top
   indices.push_back(8);
   indices.push_back(10);
   indices.push_back(9);
   indices.push_back(8);
   indices.push_back(11);
   indices.push_back(10);

   //left
   indices.push_back(16);
   indices.push_back(17);
   indices.push_back(18);
   indices.push_back(16);
   indices.push_back(18);
   indices.push_back(19);

   //back
   indices.push_back(7);
   indices.push_back(5);
   indices.push_back(6);
   indices.push_back(7);
   indices.push_back(4);
   indices.push_back(5);

   //bot
   indices.push_back(12);
   indices.push_back(14);
   indices.push_back(13);
   indices.push_back(12);
   indices.push_back(15);
   indices.push_back(14);

   //right
   indices.push_back(20);
   indices.push_back(21);
   indices.push_back(22);
   indices.push_back(20);
   indices.push_back(23);
   indices.push_back(21);

   return CreateMesh(verts, indices);
}


//-----------------------------------------------------------------------------------------------
Mesh* TheRenderer::GenerateCubeMeshPCUTB(float length, float width, float height) const
{
   const float halfLength = length / 2.f;
   const float halfWidth = width / 2.f;
   const float halfHeight = height / 2.f;

   MeshBuilder builder;
   builder.AddQuad(Vector3f(-halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, -halfLength));
   builder.AddQuad(Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, halfHeight, halfLength), Vector3f(halfWidth, halfHeight, -halfLength));
   builder.AddQuad(Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, halfHeight, halfLength), Vector3f(halfWidth, halfHeight, halfLength));
   builder.AddQuad(Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, -halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, halfLength));
   builder.AddQuad(Vector3f(-halfWidth, halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, halfLength), Vector3f(-halfWidth, halfHeight, halfLength));
   builder.AddQuad(Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(-halfWidth, -halfHeight, -halfLength));

   GLuint vboID = GenerateVBO();
   GLuint iboID = GenerateVBO();
   Mesh* mesh = new Mesh(vboID, iboID, &m_vertexPCUTB);
   builder.CopyToMesh(mesh, CopyToPCUTB);
   return mesh;
}


//-----------------------------------------------------------------------------------------------
Mesh* TheRenderer::GenerateCubeMeshPCUTBN(float length, float width, float height) const
{
   const float halfLength = length / 2.f;
   const float halfWidth = width / 2.f;
   const float halfHeight = height / 2.f;

   MeshBuilder builder;
   builder.AddQuad(Vector3f(-halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, -halfLength));
   builder.AddQuad(Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, halfHeight, halfLength), Vector3f(halfWidth, halfHeight, -halfLength));
   builder.AddQuad(Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, halfHeight, halfLength), Vector3f(halfWidth, halfHeight, halfLength));
   builder.AddQuad(Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, -halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, halfLength));
   builder.AddQuad(Vector3f(-halfWidth, halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, halfLength), Vector3f(-halfWidth, halfHeight, halfLength));
   builder.AddQuad(Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(-halfWidth, -halfHeight, -halfLength));

   GLuint vboID = GenerateVBO();
   GLuint iboID = GenerateVBO();
   Mesh* mesh = new Mesh(vboID, iboID, &m_vertexPCUTBN);
   builder.CopyToMesh(mesh, CopyToPCUTBN);
   return mesh;
}


//-----------------------------------------------------------------------------------------------
Mesh* TheRenderer::GenerateCubeMeshBONE(float length, float width, float height) const
{
   const float halfLength = length / 2.f;
   const float halfWidth = width / 2.f;
   const float halfHeight = height / 2.f;

   MeshBuilder builder;
   builder.AddQuad(Vector3f(-halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, -halfLength));
   builder.AddQuad(Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, halfHeight, halfLength), Vector3f(halfWidth, halfHeight, -halfLength));
   builder.AddQuad(Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, halfHeight, halfLength), Vector3f(halfWidth, halfHeight, halfLength));
   builder.AddQuad(Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, -halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, halfLength));
   builder.AddQuad(Vector3f(-halfWidth, halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, halfLength), Vector3f(-halfWidth, halfHeight, halfLength));
   builder.AddQuad(Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(-halfWidth, -halfHeight, -halfLength));

   GLuint vboID = GenerateVBO();
   GLuint iboID = GenerateVBO();
   Mesh* mesh = new Mesh(vboID, iboID, &m_vertexBone);
   builder.CopyToMesh(mesh, CopyToBONE);
   return mesh;
}


//-----------------------------------------------------------------------------------------------
Mesh* TheRenderer::GenerateQuadMesh(float width, float height) const
{
   const float halfWidth = width / 2.f;
   const float halfHeight = height / 2.f;
   
   MeshBuilder builder;
   builder.AddQuad(Vector3f(-halfWidth, -halfHeight, 0.f), Vector3f(halfWidth, -halfHeight, 0.f), Vector3f(halfWidth, halfHeight, 0.f), Vector3f(-halfWidth, halfHeight, 0.f));

   GLuint vboID = GenerateVBO();
   GLuint iboID = GenerateVBO();
   Mesh* mesh = new Mesh(vboID, iboID, &m_vertexPCUTB);
   builder.CopyToMesh(mesh, CopyToPCUTB);
   return mesh;
}


//-----------------------------------------------------------------------------------------------
Framebuffer* TheRenderer::CreateFramebuffer(int width, int height, const std::vector<TextureFormat>& colorFormats, TextureFormat depthStencilFormat)
{
   std::vector<Texture*> colorTargets;

   for (const TextureFormat& format : colorFormats)
   {
      colorTargets.push_back(Texture::CreateTextureWithFormat(width, height, format));
   }

   Texture* depthStencilTarget = Texture::CreateTextureWithFormat(width, height, depthStencilFormat);

   GLuint fboHandle;
   glGenFramebuffers(1, &fboHandle);
   if (fboHandle == NULL)
   {
      return nullptr;
   }

   Framebuffer *fbo = new Framebuffer(fboHandle, width, height, colorTargets, depthStencilTarget);

   // OpenGL Initialization... stuff...
   // If you bound a framebuffer to your Renderer -
   // be careful you didn't unbind just now...
   glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

   for (size_t targetIndex = 0; targetIndex < colorTargets.size(); ++targetIndex)
   {
      Texture* currentTarget = colorTargets[targetIndex];
      glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + targetIndex, currentTarget->GetTextureID(), 0);
   }

   if (depthStencilTarget != nullptr)
   {
      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTarget->GetTextureID(), 0);
   }

   GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   if (status != GL_FRAMEBUFFER_COMPLETE) 
   {
      DeleteFramebuffer(fbo);
      /*ERROR_MSG(...);*/
      return nullptr;
   }
   // Revert to old state
   glBindFramebuffer(GL_FRAMEBUFFER, NULL);

   // Return FBO
   return fbo;
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::DeleteFramebuffer(Framebuffer* fbo)
{
   //if (CRenderer::GetBoundFramebuffer() == fbo) 
   //{
   //   CRenderer::BindFramebuffer(nullptr);
   //}

   GLuint handle = fbo->GetHandle();
   glDeleteFramebuffers(1, &handle);
   delete fbo;
   BindFrameBuffer(nullptr);
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::BindFrameBuffer(Framebuffer* fbo)
{
   if (m_fbo == fbo)
   {
      return;
   }

   m_fbo = fbo;
   if (fbo == nullptr)
   {
      glBindFramebuffer(GL_FRAMEBUFFER, NULL);
      glViewport(0, 0, 1600, 900); // #TODO - fix
   }
   else
   {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo->GetHandle());
      glViewport(0, 0, fbo->GetPixelWidth(), fbo->GetPixelHeight()); // assumes rendering to entire framebuffer

      GLenum renderTargets[32];
      int colorTargetCount = fbo->GetNumColorTargets();
      for (int colorTargetIndex = 0; colorTargetIndex < colorTargetCount; ++colorTargetIndex)
      {
         renderTargets[colorTargetIndex] = GL_COLOR_ATTACHMENT0 + colorTargetIndex;
      }
      glDrawBuffers(colorTargetCount, renderTargets); // how many; array of render targets
   }
}


//-----------------------------------------------------------------------------------------------
void TheRenderer::CopyFramebufferToBack(Framebuffer* fbo) const
{
   if (fbo == nullptr) 
   {
      return;
   }

   GLuint fboh = fbo->GetHandle();
   glBindFramebuffer(GL_READ_FRAMEBUFFER, fboh);
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);

   int readWidth = fbo->GetPixelWidth();
   int readHeight = fbo->GetPixelHeight();

   // #TODO - again, fix me
   int draw_width = 1600;
   int draw_height = 900;

   glBlitFramebuffer(0, 0, // lower left corner pixel
      readWidth, readHeight, // top right corner pixel 
      0, 0, // lower left pixel of read buffer
      draw_width, draw_height, // top right pixel of read buffer
      GL_COLOR_BUFFER_BIT,
      GL_NEAREST);

   //GL_CHECK_ERROR(); #TODO - implement
}


//-----------------------------------------------------------------------------------------------
GLenum TheRenderer::GetEnumForBufferDataUsage(BufferDataUsage usage) const
{
   switch (usage)
   {
   case STATIC_DRAW: return GL_STATIC_DRAW;
   }

   return NULL;
}