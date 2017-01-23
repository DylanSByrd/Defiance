#pragma once

#include <vector>
#include <string>
#include "Engine/Math/Matrix4x4.hpp"


//-----------------------------------------------------------------------------------------------
class FileBinaryWriter;
class FileBinaryReader;


//-----------------------------------------------------------------------------------------------
struct SkeletonJoint
{
   std::string name;
   int parentJointIndex;
   Matrix4x4 boneToModelSpace;
   Matrix4x4 modelToBoneSpace;
};


//-----------------------------------------------------------------------------------------------
class Skeleton
{
public:
   ~Skeleton();

   void AddJoint(const std::string& name, int parentJointIndex, const Matrix4x4& initialBoneToModelMatrix);
   Vector3f GetJointPosition(int jointIndex) const;
   bool GetBonePositions(int jointIndex, Vector3f* jointPosition, Vector3f* parentPosition) const;
   int GetJointIndexFromName(const std::string& jointName);
   void SetJointWorldTransform(int jointIndex, const Matrix4x4& worldTransform);
   Matrix4x4* GetBoneMatrices();

   void DebugRender() const;

   size_t GetJointCount() const { return m_joints.size(); }
   int GetLastAddedJointIndex() const { return m_joints.size() - 1; }
   const std::string& GetNameOfJoint(int jointIndex) const { return m_joints[jointIndex]->name; }
   SkeletonJoint* GetJointAtIndex(unsigned int index) { return m_joints[index]; }

   // FileIO
   static const uint8_t FILE_VERSION;
   
   void WriteToFile(const std::string& fileName);
   bool ReadFromFile(const std::string& fileName);

private:
   std::vector<SkeletonJoint*> m_joints;

   // FileIO
   void WriteToStream(FileBinaryWriter* writer);
   void WriteJointNames(FileBinaryWriter* writer);
   void WriteJointParentIndices(FileBinaryWriter* writer);
   void WriteJointInitialModelSpaces(FileBinaryWriter* writer);
   void ReadFromStream(FileBinaryReader* reader);
   void ReadJointNames(FileBinaryReader* reader);
   void ReadJointParentIndices(FileBinaryReader* reader);
   void ReadJointInitialModelSpaces(FileBinaryReader* reader);
};