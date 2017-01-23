#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
Material::~Material()
{
   if (m_program.IsValid())
   {
      ClearAllUniformData();

      for (auto uniformIter = m_uniforms.begin(); uniformIter != m_uniforms.end(); ++uniformIter)
      {
         delete (uniformIter->second);
      }

      g_theRenderer->DeleteProgram(m_program);
   }

}


//-----------------------------------------------------------------------------------------------
void Material::ClearAllUniformData()
{
   for (auto uniformIter = m_uniforms.begin(); uniformIter != m_uniforms.end(); ++uniformIter)
   {
      ElementType uniformType = uniformIter->second->type;
      switch (uniformType)
      {
      case INT_ELEMENT:
      {
         delete[] (int*)uniformIter->second->data;
         break;
      }
      case FLOAT_ELEMENT:
      {
         delete[] (float*)uniformIter->second->data;
         break;
      }
      case VEC3_ELEMENT:
      {
         delete[] (Vector3f*)uniformIter->second->data;
         break;
      }
      case VEC4_ELEMENT:
      {
         delete[] (Vector4f*)uniformIter->second->data;
         break;
      }
      case MAT4_ELEMENT:
      {
         delete[] (float*)uniformIter->second->data;
         break;
      }
      }
   }
}


//-----------------------------------------------------------------------------------------------
UniformData* Material::CreateOrGetUniformData(const std::string& name)
{
   // If we have it, return it
   auto uniformIter = m_uniforms.find(name);
   if (uniformIter != m_uniforms.end())
   {
      return uniformIter->second;
   }

   // We don't, so make it
   GLint bindPos = m_program.GetUniformBindPositionByName(name);
   if (bindPos == -1)
   {
      return nullptr;
   }

   UniformData* newUniform = new UniformData;
   newUniform->bindPos = bindPos;
   m_uniforms[name] = newUniform;

   return newUniform;
}


//-----------------------------------------------------------------------------------------------
const UniformData* Material::GetUniformByName(const std::string& name) const
{
   auto uniformIter = m_uniforms.find(name);
   ASSERT_OR_DIE(uniformIter != m_uniforms.end(), Stringf("Uniform %s not found!\n", name.c_str()));
   return uniformIter->second;
}


//-----------------------------------------------------------------------------------------------
void Material::AddTexture(const std::string& name, const Texture* tex)
{
   // Add/Replace texture
   m_textures[name] = tex;

   // Find the uniform. 
   UniformData* uniformData = CreateOrGetUniformData(name);
   if (uniformData == nullptr)
   {
      return;
   }
      
   // Bind to uniform
   uniformData->type = SAMPLER2D_ELEMENT;
   uniformData->elementCount = 1;
}


//-----------------------------------------------------------------------------------------------
void Material::SetVec3(const std::string& name, const Vector3f& vec)
{
   Vector3f vecToAdd = vec;
   SetMultiVec3(name, 1, &vecToAdd);
}


//-----------------------------------------------------------------------------------------------
void Material::SetVec4(const std::string& name, const Vector4f& vec)
{
   Vector4f vecToAdd = vec;
   SetMultiVec4(name, 1, &vecToAdd);
}


//-----------------------------------------------------------------------------------------------
void Material::SetMat4(const std::string& name, const Matrix4x4& mat)
{
   Matrix4x4 matToAdd = mat;
   SetMultiMat4(name, 1, &matToAdd);
}


//-----------------------------------------------------------------------------------------------
void Material::SetInt(const std::string& name, int input)
{
   SetMultiInt(name, 1, &input);
}


//-----------------------------------------------------------------------------------------------
void Material::SetFloat(const std::string& name, float input)
{
   SetMultiFloat(name, 1, &input);
}


//-----------------------------------------------------------------------------------------------
void Material::SetColor(const std::string& name, const Rgba& color)
{
   Vector4f normalizedColor = color.GetNormalized();
   SetMultiVec4(name, 1, &normalizedColor);
}


//-----------------------------------------------------------------------------------------------
void Material::SetMultiVec3(const std::string& name, int count, const Vector3f* vecs)
{
   // Find the uniform
   UniformData* uniformData = CreateOrGetUniformData(name);
   if (uniformData == nullptr)
   {
      return;
   }

   // delete old data if present
   if (uniformData->data != nullptr)
   {
      delete[] (Vector3f*)uniformData->data;
   }

   // give new data
   uniformData->type = VEC3_ELEMENT;
   uniformData->elementCount = count;

   Vector3f* data = new Vector3f[count];
   for (int inputIndex = 0; inputIndex < count; ++inputIndex)
   {
      data[inputIndex] = vecs[inputIndex];
   }

   uniformData->data = data;
}


//-----------------------------------------------------------------------------------------------
void Material::SetMultiVec4(const std::string& name, int count, const Vector4f* vecs)
{
   // Find the uniform
   UniformData* uniformData = CreateOrGetUniformData(name);
   if (uniformData == nullptr)
   {
      return;
   }

   // delete old data if present
   if (uniformData->data != nullptr)
   {
      delete[] (Vector4f*)uniformData->data;
   }

   // give new data
   uniformData->type = VEC4_ELEMENT;
   uniformData->elementCount = count;

   Vector4f* data = new Vector4f[count];
   for (int inputIndex = 0; inputIndex < count; ++inputIndex)
   {
      data[inputIndex] = vecs[inputIndex];
   }

   uniformData->data = data;
}


//-----------------------------------------------------------------------------------------------
void Material::SetMultiMat4(const std::string& name, int count, const Matrix4x4* mats)
{
   // Find the uniform
   UniformData* uniformData = CreateOrGetUniformData(name);
   if (uniformData == nullptr)
   {
      return;
   }

   // delete old data if present
   if (uniformData->data != nullptr)
   {
      delete[] (float*)uniformData->data;
   }

   // give new data
   const int ELEMENTS_IN_MAT44 = 16;
   uniformData->type = MAT4_ELEMENT;
   uniformData->elementCount = count;

   // give new data
   float* data = new float[count * ELEMENTS_IN_MAT44];
   for (int inputIndex = 0; inputIndex < count; ++inputIndex)
   {
      memcpy(&data[inputIndex * ELEMENTS_IN_MAT44], &mats[inputIndex].data, sizeof(mats[inputIndex].data));
   }

   uniformData->data = data;
}


//-----------------------------------------------------------------------------------------------
void Material::SetMultiInt(const std::string& name, int count, int* inputs)
{
   // Find the uniform
   UniformData* uniformData = CreateOrGetUniformData(name);
   if (uniformData == nullptr)
   {
      return;
   }

   // delete old data if present
   if (uniformData->data != nullptr)
   {
      delete[] (int*)uniformData->data;
   }

   // give new data
   uniformData->type = INT_ELEMENT;
   uniformData->elementCount = count;

   // give new data
   int* data = new int[count];
   for (int inputIndex = 0; inputIndex < count; ++inputIndex)
   {
      data[inputIndex] = inputs[inputIndex];

   }

   uniformData->data = data;
}


//-----------------------------------------------------------------------------------------------
void Material::SetMultiFloat(const std::string& name, int count, float* inputs)
{
   // Find the uniform
   UniformData* uniformData = CreateOrGetUniformData(name);
   if (uniformData == nullptr)
   {
      return;
   }

   // delete old data if present
   if (uniformData->data != nullptr)
   {
      delete[] (float*)uniformData->data;
   }

   // give new data
   uniformData->type = FLOAT_ELEMENT;
   uniformData->elementCount = count;

   // give new data
   float* data = new float[count];
   for (int inputIndex = 0; inputIndex < count; ++inputIndex)
   {
      data[inputIndex] = inputs[inputIndex];

   }

   uniformData->data = data;
}


//-----------------------------------------------------------------------------------------------
void Material::SetMultiColor(const std::string& name, int count, const Rgba* colors)
{
   std::vector<Vector4f> normalizedColors;
   normalizedColors.reserve(count);
   for (int colorIndex = 0; colorIndex < count; ++colorIndex)
   {
      normalizedColors.push_back(colors[colorIndex].GetNormalized());
   }

   SetMultiVec4(name, count, &normalizedColors[0]);
}


//-----------------------------------------------------------------------------------------------
void Material::SetLight(const std::string& name, const Light* light)
{
   SetVec3(name + "Position", light->position);
   SetVec3(name + "Direction", light->direction);
   SetColor(name + "Color", light->color);
   SetInt(name + "IsDirectionalLight", light->isDirectionalLight);
   SetFloat(name + "MinLightDistance", light->minLightDistance);
   SetFloat(name + "MaxLightDistance", light->maxLightDistance);
   SetFloat(name + "PowerAtMinDistance", light->powerAtMinDistance);
   SetFloat(name + "PowerAtMaxDistance", light->powerAtMaxDistance);
   SetFloat(name + "InnerCosAngle", light->innerCosAngle);
   SetFloat(name + "OuterCosAngle", light->outerCosAngle);
   SetFloat(name + "PowerInsideInnerAngle", light->powerInsideInnerAngle);
   SetFloat(name + "PowerOutsideOuterAngle", light->powerOutsideOuterAngle);
}


//-----------------------------------------------------------------------------------------------
void Material::SetMultiLights(const std::string& name, int count, const Light* lights)
{
   Vector3f positions[16];
   Vector3f directions[16];
   Rgba colors[16];
   int directionLightBools[16];
   float minLightDists[16];
   float maxLightDists[16];
   float minPowers[16];
   float maxPowers[16];
   float innerAngles[16];
   float outerAngles[16];
   float innerPowers[16];
   float outerPowers[16];

   for (int lightIndex = 0; lightIndex < count; ++lightIndex)
   {
      positions[lightIndex] = lights[lightIndex].position;
      directions[lightIndex] = lights[lightIndex].direction;
      colors[lightIndex] = lights[lightIndex].color;
      directionLightBools[lightIndex] = lights[lightIndex].isDirectionalLight;
      minLightDists[lightIndex] = lights[lightIndex].minLightDistance;
      maxLightDists[lightIndex] = lights[lightIndex].maxLightDistance;
      minPowers[lightIndex] = lights[lightIndex].powerAtMinDistance;
      maxPowers[lightIndex] = lights[lightIndex].powerAtMaxDistance;
      innerAngles[lightIndex] = lights[lightIndex].innerCosAngle;
      outerAngles[lightIndex] = lights[lightIndex].outerCosAngle;
      innerPowers[lightIndex] = lights[lightIndex].powerInsideInnerAngle;
      outerPowers[lightIndex] = lights[lightIndex].powerOutsideOuterAngle;
   }

   SetInt("uLightCount", count);
   SetMultiVec3(name + "Position[0]", count, positions);
   SetMultiVec3(name + "Direction[0]", count, directions);
   SetMultiColor(name + "Color[0]", count, colors);
   SetMultiInt(name + "IsDirectionalLight[0]", count, directionLightBools);
   SetMultiFloat(name + "MinLightDistance[0]", count, minLightDists);
   SetMultiFloat(name + "MaxLightDistance[0]", count, maxLightDists);
   SetMultiFloat(name + "PowerAtMinDistance[0]", count, minPowers);
   SetMultiFloat(name + "PowerAtMaxDistance[0]", count, maxPowers);
   SetMultiFloat(name + "InnerCosAngle[0]", count, innerAngles);
   SetMultiFloat(name + "OuterCosAngle[0]", count, outerAngles);
   SetMultiFloat(name + "PowerInsideInnerAngle[0]", count, innerPowers);
   SetMultiFloat(name + "PowerOutsideOuterAngle[0]", count, outerPowers);
}


//-----------------------------------------------------------------------------------------------
void Material::SetPositionMatrices(const Matrix4x4& model, const Matrix4x4& view, const Matrix4x4& projection)
{
   SetMat4("uModel", model);
   SetMat4("uView", view);
   SetMat4("uProj", projection);
}