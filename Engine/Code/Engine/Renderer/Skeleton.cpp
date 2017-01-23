#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Math/Vector4f.hpp"
#include "Engine/Math/Vector3f.hpp"
#include "Engine/Debug/DebugRenderer.hpp"
#include "Engine/Debug/Console.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/IO/FileBinaryWriter.hpp"
#include "Engine/IO/FileBinaryReader.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const uint8_t Skeleton::FILE_VERSION = 1;


//-----------------------------------------------------------------------------------------------
Skeleton::~Skeleton()
{
   for (SkeletonJoint* joint : m_joints)
   {
      delete joint;
      joint = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
void Skeleton::AddJoint(const std::string& name, int parentJointIndex, const Matrix4x4& initialBoneToModelMatrix)
{
   SkeletonJoint* jointToAdd = new SkeletonJoint;
   jointToAdd->name = name;
   jointToAdd->parentJointIndex = parentJointIndex;
   jointToAdd->boneToModelSpace = initialBoneToModelMatrix;
   jointToAdd->modelToBoneSpace = initialBoneToModelMatrix.GetInverseOrthonormal();
   m_joints.push_back(jointToAdd);
}


//-----------------------------------------------------------------------------------------------
Vector3f Skeleton::GetJointPosition(int jointIndex) const
{
   const Matrix4x4& modelMat = m_joints[jointIndex]->boneToModelSpace;
   Vector3f position = modelMat.GetRow(3).xyz();
   return position;
}


//-----------------------------------------------------------------------------------------------
bool Skeleton::GetBonePositions(int jointIndex, Vector3f* jointPosition, Vector3f* parentPosition) const
{
   bool didJointHaveBone = true;

   // Bone with no parent (root)
   if (m_joints[jointIndex]->parentJointIndex == -1)
   {
      return !didJointHaveBone;
   }

   *jointPosition = GetJointPosition(jointIndex);
   *parentPosition = GetJointPosition(m_joints[jointIndex]->parentJointIndex);

   return didJointHaveBone;
}


//-----------------------------------------------------------------------------------------------
int Skeleton::GetJointIndexFromName(const std::string& jointName)
{
   for (size_t jointIndex = 0; jointIndex < m_joints.size(); ++jointIndex)
   {
      if (!strcmp(m_joints[jointIndex]->name.c_str(), jointName.c_str()))
      {
         return jointIndex;
      }
   }

   // Joint with name not found
   return -1;
}


//-----------------------------------------------------------------------------------------------
void Skeleton::SetJointWorldTransform(int jointIndex, const Matrix4x4& worldTransform)
{
   m_joints[jointIndex]->boneToModelSpace = worldTransform;
   //m_joints[jointIndex]->modelToBoneSpace = worldTransform.GetInverseOrthonormal();
}


//-----------------------------------------------------------------------------------------------
Matrix4x4* Skeleton::GetBoneMatrices()
{
   int boneCount = m_joints.size();
   Matrix4x4* boneMats = new Matrix4x4[boneCount];

   for (int boneIndex = 0; boneIndex < boneCount; ++boneIndex)
   {
      boneMats[boneIndex] = m_joints[boneIndex]->modelToBoneSpace * m_joints[boneIndex]->boneToModelSpace;
   }

   return boneMats;
}


//-----------------------------------------------------------------------------------------------
void Skeleton::DebugRender() const
{
   for (size_t jointIndex = 0; jointIndex < m_joints.size(); ++jointIndex)
   {
      Vector3f jointPosition;
      Vector3f parentPosition;
      bool hasBone = GetBonePositions(jointIndex, &jointPosition, &parentPosition);

      if (!hasBone)
      {
         jointPosition = GetJointPosition(jointIndex);
         g_theDebugRenderer->DrawPoint(3, jointPosition, Rgba::GREEN, 0.f);
         continue;
      }

      g_theDebugRenderer->DrawPoint(1, jointPosition, Rgba::BLUE, 0.f);
      g_theDebugRenderer->DrawLine(jointPosition, parentPosition, Rgba::YELLOW, 0.f);
   }
}


//-----------------------------------------------------------------------------------------------
void Skeleton::WriteToFile(const std::string& fileName)
{
   FileBinaryWriter writer;
   writer.OpenFile(fileName);

   WriteToStream(&writer);

   writer.CloseFile();
}


//-----------------------------------------------------------------------------------------------
void Skeleton::WriteToStream(FileBinaryWriter* writer)
{
   writer->Write(FILE_VERSION);

   writer->Write<uint32_t>(m_joints.size());

   WriteJointNames(writer);
   WriteJointParentIndices(writer);
   WriteJointInitialModelSpaces(writer);
}


//-----------------------------------------------------------------------------------------------
void Skeleton::WriteJointNames(FileBinaryWriter* writer)
{
   for (SkeletonJoint* joint : m_joints)
   {
      writer->WriteString(joint->name);
   }
}


//-----------------------------------------------------------------------------------------------
void Skeleton::WriteJointParentIndices(FileBinaryWriter* writer)
{
   for (SkeletonJoint* joint : m_joints)
   {
      writer->Write(joint->parentJointIndex);
   }
}


//-----------------------------------------------------------------------------------------------
void Skeleton::WriteJointInitialModelSpaces(FileBinaryWriter* writer)
{
   for (SkeletonJoint* joint : m_joints)
   {
      writer->Write(joint->boneToModelSpace);
   }
}


//-----------------------------------------------------------------------------------------------
bool Skeleton::ReadFromFile(const std::string& fileName)
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
void Skeleton::ReadFromStream(FileBinaryReader* reader)
{
   uint8_t incomingFileVersion;
   reader->Read(&incomingFileVersion);

   // #TODO - eventually be more backwards compatible
   ASSERT_OR_DIE(incomingFileVersion == FILE_VERSION, "Error: incoming file does not match the current file version!");

   uint32_t jointCount;
   reader->Read(&jointCount);
   m_joints.reserve(jointCount);

   for (uint32_t jointIndex = 0; jointIndex < jointCount; ++jointIndex)
   {
      m_joints.push_back(new SkeletonJoint);
   }

   ReadJointNames(reader);
   ReadJointParentIndices(reader);
   ReadJointInitialModelSpaces(reader);
}


//-----------------------------------------------------------------------------------------------
void Skeleton::ReadJointNames(FileBinaryReader* reader)
{
   for (size_t jointIndex = 0; jointIndex < m_joints.size(); ++jointIndex)
   {
      reader->ReadString(&m_joints[jointIndex]->name);
   }
}


//-----------------------------------------------------------------------------------------------
void Skeleton::ReadJointParentIndices(FileBinaryReader* reader)
{
   for (size_t jointIndex = 0; jointIndex < m_joints.size(); ++jointIndex)
   {
      reader->Read(&m_joints[jointIndex]->parentJointIndex);
   }
}


//-----------------------------------------------------------------------------------------------
void Skeleton::ReadJointInitialModelSpaces(FileBinaryReader* reader)
{
   for (size_t jointIndex = 0; jointIndex < m_joints.size(); ++jointIndex)
   {
      reader->Read(&m_joints[jointIndex]->boneToModelSpace);
      m_joints[jointIndex]->modelToBoneSpace = m_joints[jointIndex]->boneToModelSpace.GetInverseOrthonormal();
   }
}