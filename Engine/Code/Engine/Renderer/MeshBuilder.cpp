#include "Engine/Core/ByteUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/VertexDefinition.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Debug/Console.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/IO/FileBinaryWriter.hpp"
#include "Engine/IO/FileBinaryReader.hpp"
#include "Engine/Math/Vector4i.hpp"
#include "Engine/Math/Vector4f.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const uint8_t MeshBuilder::FILE_VERSION = 1;
/*
   Currently supports the following data flags:
   Position,
   Normal,
   Tangent,
   Bitangent,
   UV0,
   UV1,
   Color
*/


//-----------------------------------------------------------------------------------------------
MeshBuilder::MeshBuilder()
   : m_vertexStamp()
   , m_materialName()
   , m_startIndexOffset(0)
   , m_startVertIndex(0)
   , m_dataMask(0)
{
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::BeginDrawInstruction()
{
   m_startVertIndex = m_vertices.size();
   m_startIndexOffset = m_indices.size();
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::EndDrawInstruction()
{
   DrawInstruction instructionToAdd;

   // #TODO - combine draw calls of equal index usage and primitive type
   if (m_startIndexOffset < m_indices.size())
   {
      instructionToAdd.primitive = PRIMITIVE_TRIANGLES;
      instructionToAdd.startIndex = m_startIndexOffset;
      instructionToAdd.count = m_indices.size() - m_startIndexOffset;
      instructionToAdd.isUsingIndexBuffer = true;
      m_drawInstructions.push_back(instructionToAdd);
   }
   else if (m_startVertIndex < m_vertices.size())
   {
      instructionToAdd.primitive = PRIMITIVE_TRIANGLES;
      instructionToAdd.startIndex = m_startVertIndex;
      instructionToAdd.count = m_vertices.size() - m_startVertIndex;
      instructionToAdd.isUsingIndexBuffer = false;
      m_drawInstructions.push_back(instructionToAdd);
   } 

   //prevent double ends
   m_startVertIndex = m_vertices.size();
   m_startIndexOffset = m_indices.size();
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::SetCurrentVertexColor(const Rgba& color)
{
   m_vertexStamp.color = color;
   SetBit(m_dataMask, BIT(MESH_DATA_COLOR));
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::SetCurrentVertexTangent(const Vector3f& tangent)
{
   m_vertexStamp.tangent = tangent;
   SetBit(m_dataMask, BIT(MESH_DATA_TANGENT));
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::SetCurrentVertexBitangent(const Vector3f& bitangent)
{
   m_vertexStamp.bitangent = bitangent;
   SetBit(m_dataMask, BIT(MESH_DATA_BITANGENT));
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::SetCurrentVertexNormal(const Vector3f& normal)
{
   m_vertexStamp.normal = normal;
   SetBit(m_dataMask, BIT(MESH_DATA_NORMAL));
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::SetCurrentVertexTBN(const Vector3f& tangent, const Vector3f& bitangent, const Vector3f& normal)
{
   m_vertexStamp.tangent = tangent;
   m_vertexStamp.bitangent = bitangent;
   m_vertexStamp.normal = normal;

   SetBit(m_dataMask, BIT(MESH_DATA_TANGENT));
   SetBit(m_dataMask, BIT(MESH_DATA_BITANGENT));
   SetBit(m_dataMask, BIT(MESH_DATA_NORMAL));

}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::SetCurrentVertexUV0(const TexCoords& uv)
{
   m_vertexStamp.uv0 = uv;
   SetBit(m_dataMask, BIT(MESH_DATA_UV0));
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::SetCurrentVertexUV1(const TexCoords& uv)
{
   m_vertexStamp.uv1 = uv;
   SetBit(m_dataMask, BIT(MESH_DATA_UV1));
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::SetBoneWeights(const Vector4i& boneIndices, const Vector4f& boneWeights)
{
   m_vertexStamp.boneIndices = boneIndices;
   m_vertexStamp.boneWeights = boneWeights;
   SetBit(m_dataMask, BIT(MESH_DATA_BONE_WEIGHTS));
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::RenormalizeSkinWeights()
{
   Vector4f& weights = m_vertexStamp.boneWeights;
   float totalWeight = weights.x + weights.y + weights.z + weights.w;
   if (totalWeight > 0.f)
   {
      weights /= totalWeight;
   }
   else
   {
      weights = Vector4f(1.f, 0.f, 0.f, 0.f);
   }
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::SetCurrentVertexPosition(const Vector3f& position)
{
   m_vertexStamp.position = position;
   SetBit(m_dataMask, BIT(MESH_DATA_POSITION));
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::AddCurrentVertexToMesh()
{
   m_vertices.push_back(m_vertexStamp);
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::ClearBoneWeights()
{
   m_vertexStamp.boneIndices = Vector4i(0, 0, 0, 0);
   m_vertexStamp.boneWeights = Vector4f(1.f, 0.f, 0.f, 0.f);
   ClearBit(m_dataMask, BIT(MESH_DATA_BONE_WEIGHTS));
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::AddIndex(int index)
{
   m_indices.push_back(index);
}


//-----------------------------------------------------------------------------------------------
bool MeshBuilder::IsSkinned() const
{
   return IsBitSet(m_dataMask, BIT(MESH_DATA_BONE_WEIGHTS));
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::AddTriangleIndices(int botLeftIndex, int rightIndex, int topIndex)
{
   m_indices.push_back(botLeftIndex);
   m_indices.push_back(rightIndex);
   m_indices.push_back(topIndex);
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::AddQuadIndices(int botLeftIndex, int botRightIndex, int topRightIndex, int topLeftIndex)
{
   m_indices.push_back(botLeftIndex);
   m_indices.push_back(botRightIndex);
   m_indices.push_back(topRightIndex);
   
   m_indices.push_back(botLeftIndex);
   m_indices.push_back(topRightIndex);
   m_indices.push_back(topLeftIndex);
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::AddTriangle(const Vector3f& botLeft, const Vector3f& right, const Vector3f& top)
{
   Vector3f tangent = (right - botLeft).GetNormalized();
   Vector3f bitangent = (top - botLeft).GetNormalized();
   Vector3f normal = bitangent.CrossProduct(tangent);
   
   BeginDrawInstruction();

   SetCurrentVertexNormal(normal);
   SetCurrentVertexTangent(tangent);
   SetCurrentVertexBitangent(bitangent);

   SetCurrentVertexUV0(Vector2f(0.f, 1.f));
   SetCurrentVertexPosition(botLeft);
   AddCurrentVertexToMesh();

   SetCurrentVertexUV0(Vector2f(1.f, 1.f));
   SetCurrentVertexPosition(right);
   AddCurrentVertexToMesh();

   SetCurrentVertexUV0(Vector2f(0.f, 0.f));
   SetCurrentVertexPosition(top);
   AddCurrentVertexToMesh();

   AddTriangleIndices(m_startVertIndex + 0, m_startVertIndex + 1, m_startVertIndex + 2);

   EndDrawInstruction();
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::AddQuad(const Vector3f& botLeft, const Vector3f& botRight, const Vector3f& topRight, const Vector3f& topLeft)
{
   Vector3f tangent = (botRight - botLeft).GetNormalized();
   Vector3f bitangent = (topLeft - botLeft).GetNormalized();
   Vector3f normal = bitangent.CrossProduct(tangent);

   BeginDrawInstruction();

   SetCurrentVertexNormal(normal);
   SetCurrentVertexTangent(tangent);
   SetCurrentVertexBitangent(bitangent);

   SetCurrentVertexUV0(Vector2f(0.f, 1.f));
   SetCurrentVertexPosition(botLeft);
   AddCurrentVertexToMesh();

   SetCurrentVertexUV0(Vector2f(1.f, 1.f));
   SetCurrentVertexPosition(botRight);
   AddCurrentVertexToMesh();

   SetCurrentVertexUV0(Vector2f(1.f, 0.f));
   SetCurrentVertexPosition(topRight);
   AddCurrentVertexToMesh();
   
   SetCurrentVertexUV0(Vector2f(0.f, 0.f));
   SetCurrentVertexPosition(topLeft);
   AddCurrentVertexToMesh();

   AddQuadIndices(m_startVertIndex + 0, m_startVertIndex + 1, m_startVertIndex + 2, m_startVertIndex + 3);

   EndDrawInstruction();
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::BuildCubePCUTB(float length, float width, float height)
{
   const float halfLength = length / 2.f;
   const float halfWidth = width / 2.f;
   const float halfHeight = height / 2.f;
   
   AddQuad(Vector3f(-halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, -halfLength));
   AddQuad(Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, halfHeight, halfLength), Vector3f(halfWidth, halfHeight, -halfLength));
   AddQuad(Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, halfHeight, halfLength), Vector3f(halfWidth, halfHeight, halfLength));
   AddQuad(Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(-halfWidth, -halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, -halfLength), Vector3f(-halfWidth, halfHeight, halfLength));
   AddQuad(Vector3f(-halfWidth, halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, -halfLength), Vector3f(halfWidth, halfHeight, halfLength), Vector3f(-halfWidth, halfHeight, halfLength));
   AddQuad(Vector3f(-halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, -halfHeight, halfLength), Vector3f(halfWidth, -halfHeight, -halfLength), Vector3f(-halfWidth, -halfHeight, -halfLength));
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::CopyToMesh(Mesh* mesh, VertexCopyFunc* CopyFunc)
{
   const int vertexCount = m_vertices.size();
   if (mesh == nullptr || vertexCount == 0)
   {
      return;
   }

   const VertexDefinition* vertDef = mesh->GetVertexDefinition();
   const int vertexSize = vertDef->GetSizeOfVertex();
   const int totalSizeOfVertices = vertexCount * vertexSize;
   unsigned char* vertByteBuffer = new unsigned char[totalSizeOfVertices];

   unsigned char* copyDestination = vertByteBuffer;
   for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
   {
      CopyFunc(copyDestination, m_vertices[vertexIndex]);
      copyDestination += vertexSize;
   }
   mesh->WriteToVBO(vertByteBuffer, vertexCount);
   mesh->WriteToIBO(m_indices);
   mesh->SetDrawInstructions(m_drawInstructions);

   // Cleanup
   delete[] vertByteBuffer;
}


// #TODO 
// - make a combine builders that takes a vector of MeshBuilder*'s
// - take materialID into account
//-----------------------------------------------------------------------------------------------
void MeshBuilder::AppendMeshBuilder(MeshBuilder* otherBuilder)
{
//    int numVertices = m_vertices.size();
// 
//    for (size_t vertexIndex = 0; vertexIndex < otherBuilder->m_indices.size(); ++vertexIndex)
//    {
//       otherBuilder->m_indices[vertexIndex] += numVertices;
//    }

   /*m_vertices.insert(m_vertices.end(), otherBuilder->m_vertices.begin(), otherBuilder->m_vertices.end());
   m_indices.insert(m_indices.end(), otherBuilder->m_indices.begin(), otherBuilder->m_indices.end());
   m_drawInstructions.insert(m_drawInstructions.end(), otherBuilder->m_drawInstructions.begin(), otherBuilder->m_drawInstructions.end());*/

   int startVertIndex = GetVertexCount();
   for (const VertexMaster& vert : otherBuilder->m_vertices)
   {
      m_vertices.push_back(vert);
   }

   int startIndex = m_indices.size();
   for (int index : otherBuilder->m_indices)
   {
      m_indices.push_back(index + startVertIndex);
   }

   for (const DrawInstruction& instruction : otherBuilder->m_drawInstructions)
   {
      DrawInstruction newInstruction = instruction;
      newInstruction.isUsingIndexBuffer = true;
      newInstruction.startIndex = startIndex;
      newInstruction.count = otherBuilder->GetIndexCount();
      m_drawInstructions.push_back(newInstruction);
   }

   m_dataMask |= otherBuilder->m_dataMask;
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::CombineDrawInstructions()
{
   std::vector<DrawInstruction>::iterator instructionIter;
   for (instructionIter = m_drawInstructions.begin(); instructionIter != m_drawInstructions.end();)
   {
      DrawInstruction& instruction = *instructionIter;
      DrawInstruction& nextInstruction = *(instructionIter + 1);
      
      if ((instruction.primitive == nextInstruction.primitive)
         && (instruction.isUsingIndexBuffer == nextInstruction.isUsingIndexBuffer))
      {
         instruction.count;
      }
   }
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::Reduce()
{
   short indexCount = 0;
   std::vector<VertexMaster> reducedVerticies;
   std::vector<DrawInstruction> reducedInstructions;
   reducedVerticies.reserve(m_vertices.size());
   m_indices.clear();
   m_indices.reserve(m_vertices.size());

   for (size_t instructionIndex = 0; instructionIndex < m_drawInstructions.size(); ++instructionIndex)
   {
      DrawInstruction& instruction = m_drawInstructions[instructionIndex];
      //if (instructionIndex > 0)
      //{
      //   DrawInstruction& previousInstruction = m_drawInstructions
      //}

      instruction.isUsingIndexBuffer = true;
      instruction.startIndex = reducedVerticies.size();
      for (unsigned int vertIndex = 0; vertIndex < instruction.count; ++vertIndex)
      {
         bool notFound = true;
         for (int reducedIndex = (int)reducedVerticies.size() - 1; notFound && reducedIndex >= 0; --reducedIndex)
         {
            if (reducedVerticies[reducedIndex] == (m_vertices[vertIndex]))
            {
               m_indices.push_back((short)reducedIndex);
               notFound = false;
            }
         }
         if (notFound)
         {
            reducedVerticies.push_back(m_vertices[vertIndex]);
            m_indices.push_back((short)indexCount);
            ++indexCount;
         }
      }
   }

   reducedVerticies.shrink_to_fit();
   m_vertices = reducedVerticies;
}


// FileIO
//-----------------------------------------------------------------------------------------------
void MeshBuilder::WriteToFile(const std::string& fileName)
{
   FileBinaryWriter writer;
   writer.OpenFile(fileName);

   WriteToStream(&writer);

   writer.CloseFile();
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::WriteToStream(FileBinaryWriter* writer)
{
   writer->Write(FILE_VERSION);
 
   writer->WriteString(m_materialName);

   WriteDataMask(writer);
   
   writer->Write<uint32_t>(m_vertices.size());
   writer->Write<uint32_t>(m_indices.size());
   writer->Write<uint32_t>(m_drawInstructions.size());

   WriteVertices(writer);
   WriteIndices(writer);
   WriteDrawInstructions(writer);
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::WriteDataMask(FileBinaryWriter* writer)
{
   if (IsBitSet(m_dataMask, BIT(MESH_DATA_POSITION)))
   {
      writer->WriteString("position");
   }

   if (IsBitSet(m_dataMask, BIT(MESH_DATA_NORMAL)))
   {
      writer->WriteString("normal");
   }

   if (IsBitSet(m_dataMask, BIT(MESH_DATA_TANGENT)))
   {
      writer->WriteString("tangent");
   }

   if (IsBitSet(m_dataMask, BIT(MESH_DATA_BITANGENT)))
   {
      writer->WriteString("bitangent");
   }

   if (IsBitSet(m_dataMask, BIT(MESH_DATA_UV0)))
   {
      writer->WriteString("uv0");
   }

   if (IsBitSet(m_dataMask, BIT(MESH_DATA_UV1)))
   {
      writer->WriteString("uv1");
   }

   if (IsBitSet(m_dataMask, BIT(MESH_DATA_COLOR)))
   {
      writer->WriteString("color");
   }

   if (IsBitSet(m_dataMask, BIT(MESH_DATA_BONE_WEIGHTS)))
   {
      writer->WriteString("bones");
   }

   writer->WriteString("");
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::WriteVertices(FileBinaryWriter* writer)
{
   for (const VertexMaster& vert : m_vertices)
   {
      if (IsBitSet(m_dataMask, BIT(MESH_DATA_POSITION)))
      {
         writer->Write(vert.position);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_NORMAL)))
      {
         writer->Write(vert.normal);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_TANGENT)))
      {
         writer->Write(vert.tangent);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_BITANGENT)))
      {
         writer->Write(vert.bitangent);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_UV0)))
      {
         writer->Write(vert.uv0);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_UV1)))
      {
         writer->Write(vert.uv1);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_COLOR)))
      {
         writer->Write(vert.color);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_BONE_WEIGHTS)))
      {
         writer->Write(vert.boneIndices);
         writer->Write(vert.boneWeights);
      }
   }
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::WriteIndices(FileBinaryWriter* writer)
{
   for (size_t indicesIndex = 0; indicesIndex < m_indices.size(); ++indicesIndex)
   {
      writer->Write(m_indices[indicesIndex]);
   }
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::WriteDrawInstructions(FileBinaryWriter* writer)
{
   for (const DrawInstruction& instruction : m_drawInstructions)
   {
      //writer->Write(instruction);
      writer->Write(instruction.primitive);
      writer->Write(instruction.startIndex);
      writer->Write(instruction.count);
      writer->Write(instruction.isUsingIndexBuffer);
   }
}


//-----------------------------------------------------------------------------------------------
bool MeshBuilder::ReadFromFile(const std::string& fileName)
{
   bool wasReadSuccessful = true;

   FileBinaryReader reader;
   if (!reader.OpenFile(fileName))
   {
      g_theConsole->ConsolePrintf(Stringf("Error: Could not find file %s", fileName.c_str()), Rgba::RED);
      return !wasReadSuccessful;
   }

   ReadFromStream(&reader);

   reader.CloseFile();
   return wasReadSuccessful;
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::ReadFromStream(FileBinaryReader* reader)
{
   uint8_t incomingFileVersion;
   reader->Read(&incomingFileVersion);

   // #TODO - eventually be more backwards compatible
   ASSERT_OR_DIE(incomingFileVersion == FILE_VERSION, "Error: incoming file does not match the current file version!");

   reader->ReadString(&m_materialName);

   ReadDataMask(reader);

   uint32_t vertCount;
   uint32_t indicesCount;
   uint32_t instructionCount;
   reader->Read<uint32_t>(&vertCount);
   reader->Read<uint32_t>(&indicesCount);
   reader->Read<uint32_t>(&instructionCount);

   ReadVertices(reader, vertCount);
   ReadIndices(reader, indicesCount);
   ReadDrawInstructions(reader, instructionCount);
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::ReadDataMask(FileBinaryReader* reader)
{
   //Clear mask
   m_dataMask = 0;

   std::string flagString;
   reader->ReadString(&flagString);

   while (strcmp(flagString.c_str(), "") != 0)
   {
      if (strcmp(flagString.c_str(), "position") == 0)
      {
         SetBit(m_dataMask, BIT(MESH_DATA_POSITION));
         reader->ReadString(&flagString);
      }

      if (strcmp(flagString.c_str(), "normal") == 0)
      {
         SetBit(m_dataMask, BIT(MESH_DATA_NORMAL));
         reader->ReadString(&flagString);
      }

      if (strcmp(flagString.c_str(), "tangent") == 0)
      {
         SetBit(m_dataMask, BIT(MESH_DATA_TANGENT));
         reader->ReadString(&flagString);
      }

      if (strcmp(flagString.c_str(), "bitangent") == 0)
      {
         SetBit(m_dataMask, BIT(MESH_DATA_BITANGENT));
         reader->ReadString(&flagString);
      }

      if (strcmp(flagString.c_str(), "uv0") == 0)
      {
         SetBit(m_dataMask, BIT(MESH_DATA_UV0));
         reader->ReadString(&flagString);
      }

      if (strcmp(flagString.c_str(), "uv1") == 0)
      {
         SetBit(m_dataMask, BIT(MESH_DATA_UV1));
         reader->ReadString(&flagString);
      }

      if (strcmp(flagString.c_str(), "color") == 0)
      {
         SetBit(m_dataMask, BIT(MESH_DATA_COLOR));
         reader->ReadString(&flagString);
      }

      if (strcmp(flagString.c_str(), "bones") == 0)
      {
         SetBit(m_dataMask, BIT(MESH_DATA_BONE_WEIGHTS));
         reader->ReadString(&flagString);
      }
   }
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::ReadVertices(FileBinaryReader* reader, uint32_t count)
{
   m_vertices.clear();
   m_vertices.reserve(count);

   for (size_t vertIndex = 0; vertIndex < count; ++vertIndex)
   {
      VertexMaster vert;
      if (IsBitSet(m_dataMask, BIT(MESH_DATA_POSITION)))
      {
         reader->Read(&vert.position);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_NORMAL)))
      {
         reader->Read(&vert.normal);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_TANGENT)))
      {
         reader->Read(&vert.tangent);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_BITANGENT)))
      {
         reader->Read(&vert.bitangent);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_UV0)))
      {
         reader->Read(&vert.uv0);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_UV1)))
      {
         reader->Read(&vert.uv1);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_COLOR)))
      {
         reader->Read(&vert.color);
      }

      if (IsBitSet(m_dataMask, BIT(MESH_DATA_BONE_WEIGHTS)))
      {
         reader->Read(&vert.boneIndices);
         reader->Read(&vert.boneWeights);
      }

      m_vertices.push_back(vert);
   }
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::ReadIndices(FileBinaryReader* reader, uint32_t count)
{
   m_indices.clear();
   m_indices.resize(count);

   for (size_t index = 0; index < count; ++index)
   {
      reader->Read(&m_indices[index]);
   }
}


//-----------------------------------------------------------------------------------------------
void MeshBuilder::ReadDrawInstructions(FileBinaryReader* reader, uint32_t count)
{
   m_drawInstructions.clear();
   m_drawInstructions.reserve(count);

   for (size_t instructionIndex = 0; instructionIndex < count; ++instructionIndex)
   {
      DrawInstruction instruction;
      //reader->Read(&instruction);
      reader->Read(&instruction.primitive);
      reader->Read(&instruction.startIndex);
      reader->Read(&instruction.count);
      reader->Read(&instruction.isUsingIndexBuffer);
      m_drawInstructions.push_back(instruction);
   }

}


//-----------------------------------------------------------------------------------------------
void CopyToPCT(void* destination, const VertexMaster& source)
{
   Vertex_PCT* vert = (Vertex_PCT*)destination;
   vert->position = source.position;
   vert->color = source.color;
   vert->texCoords = source.uv0;
}


//-----------------------------------------------------------------------------------------------
void CopyToPCUTB(void* destination, const VertexMaster& source)
{
   Vertex_PCUTB* vert = (Vertex_PCUTB*)destination;
   vert->position = source.position;
   vert->color = source.color;
   vert->uv = source.uv0;
   vert->tangent = source.tangent;
   vert->bitangent = source.bitangent;
}


//-----------------------------------------------------------------------------------------------
void CopyToPCUTBN(void* destination, const VertexMaster& source)
{
   Vertex_PCUTBN* vert = (Vertex_PCUTBN*)destination;
   vert->position = source.position;
   vert->color = source.color;
   vert->uv = source.uv0;
   vert->tangent = source.tangent;
   vert->bitangent = source.bitangent;
   vert->normal = source.normal;
}


//-----------------------------------------------------------------------------------------------
void CopyToBONE(void* destination, const VertexMaster& source)
{
   Vertex_Bone* vert = (Vertex_Bone*)destination;
   vert->position = source.position;
   vert->color = source.color;
   vert->uv = source.uv0;
   vert->tangent = source.tangent;
   vert->bitangent = source.bitangent;
   vert->normal = source.normal;
   vert->boneIndices = source.boneIndices;
   vert->boneWeights = source.boneWeights;
}