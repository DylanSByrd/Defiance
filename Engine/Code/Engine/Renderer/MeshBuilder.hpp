#pragma once

#include <string>
#include <vector>
#include "Engine/Renderer/RendererCommon.hpp"
#include "Engine/Renderer/DrawInstruction.hpp"


//-----------------------------------------------------------------------------------------------
class Mesh;
class FileBinaryWriter;
class FileBinaryReader;
class ConsoleCommandsArgs;


//-----------------------------------------------------------------------------------------------
typedef void (VertexCopyFunc)(void* destination, const VertexMaster& source);


//-----------------------------------------------------------------------------------------------
enum MeshDataFlag
{
   MESH_DATA_POSITION = 0,
   MESH_DATA_NORMAL,
   MESH_DATA_TANGENT,
   MESH_DATA_BITANGENT,
   MESH_DATA_UV0,
   MESH_DATA_UV1,
   MESH_DATA_COLOR,
   MESH_DATA_BONE_WEIGHTS,
   NUM_MESH_DATA_FLAGS,
};


//-----------------------------------------------------------------------------------------------
class MeshBuilder
{
public:
   MeshBuilder();
   ~MeshBuilder() {}

   void BeginDrawInstruction();
   void EndDrawInstruction();

   void SetCurrentVertexColor(const Rgba& color);
   void SetCurrentVertexTangent(const Vector3f& tangent);
   void SetCurrentVertexBitangent(const Vector3f& bitangent);
   void SetCurrentVertexNormal(const Vector3f& normal);
   void SetCurrentVertexTBN(const Vector3f& tangent, const Vector3f& bitangent, const Vector3f& normal);
   void SetCurrentVertexUV0(const TexCoords& uv);
   void SetCurrentVertexUV1(const TexCoords& uv);
   void SetBoneWeights(const Vector4i& boneIndices, const Vector4f& boneWeights);
   void RenormalizeSkinWeights();
   void SetCurrentVertexPosition(const Vector3f& position);
   void AddCurrentVertexToMesh();
   
   void ClearBoneWeights();
   void AddIndex(int index);
   bool IsSkinned() const;
   void AddTriangleIndices(int botLeftIndex, int rightIndex, int topIndex);
   void AddQuadIndices(int botLeftIndex, int botRightIndex, int topRightIndex, int topLeftIndex);
   void AddTriangle(const Vector3f& botLeft, const Vector3f& right, const Vector3f& top);
   void AddQuad(const Vector3f& botLeft, const Vector3f& botRight, const Vector3f& topRight, const Vector3f& topLeft);

   // #TODO - find a better place!
   void BuildCubePCUTB(float length, float width, float height);

   void CopyToMesh(Mesh* mesh, VertexCopyFunc* CopyFunc);

   void SetMaterialName(const std::string& name) { m_materialName = name; }
   void AppendMeshBuilder(MeshBuilder* otherBuilder);
   void CombineDrawInstructions();
   void Reduce();

   unsigned int GetVertexIndex() const { return m_startVertIndex; }
   int GetVertexCount() const { return m_vertices.size(); }
   int GetIndexCount() const { return m_indices.size(); }


   // FileIO
   static const uint8_t FILE_VERSION;

   void WriteToFile(const std::string& fileName);
   bool ReadFromFile(const std::string& fileName);


private:
   VertexMaster m_vertexStamp;
   std::vector<VertexMaster> m_vertices;
   std::vector<int> m_indices;
   std::vector<DrawInstruction> m_drawInstructions;
   std::string m_materialName;
   unsigned int m_startIndexOffset;
   unsigned int m_startVertIndex;
   unsigned char m_dataMask;


   // FileIO
   void WriteToStream(FileBinaryWriter* writer);
   void WriteDataMask(FileBinaryWriter* writer);
   void WriteVertices(FileBinaryWriter* writer);
   void WriteIndices(FileBinaryWriter* writer);
   void WriteDrawInstructions(FileBinaryWriter* writer);
   void ReadFromStream(FileBinaryReader* reader);
   void ReadDataMask(FileBinaryReader* reader);
   void ReadVertices(FileBinaryReader* reader, uint32_t count);
   void ReadIndices(FileBinaryReader* reader, uint32_t count);
   void ReadDrawInstructions(FileBinaryReader* reader, uint32_t count);
};


// Copy Funcs
//-----------------------------------------------------------------------------------------------
void CopyToPCT(void* destination, const VertexMaster& source);
void CopyToPCUTB(void* destination, const VertexMaster& source);
void CopyToPCUTBN(void* destination, const VertexMaster& source);
void CopyToBONE(void* destination, const VertexMaster& source);