#pragma once

#include <vector>
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Renderer/Motion.hpp"


//-----------------------------------------------------------------------------------------------
class SceneImport
{
public:
   SceneImport() {}
   ~SceneImport();

   void AddMesh(MeshBuilder* newMesh) { m_meshes.push_back(newMesh); }
   void AddSkeleton(Skeleton* newSkeleton) { m_skeletons.push_back(newSkeleton); }
   void AddMotion(Motion* newMotion) { m_motions.push_back(newMotion); }
   int GetMeshCount() const { return m_meshes.size(); }
   int GetSkeletonCount() const { return m_skeletons.size(); }
   MeshBuilder* GetMesh() { return m_meshes[0]; }
   Skeleton* GetSkeleton() { return (m_skeletons.size() == 0 ? nullptr : m_skeletons[0]); }
   Skeleton* GetSkeleton(int skeletonIndex) { return m_skeletons[skeletonIndex]; }
   Motion* GetMotion() { return (m_motions.size() == 0 ? nullptr : m_motions[0]); }
   MeshBuilder* CombineMeshes();

private:
   std::vector<MeshBuilder*> m_meshes;
   std::vector<Skeleton*> m_skeletons;
   std::vector<Motion*> m_motions;
};