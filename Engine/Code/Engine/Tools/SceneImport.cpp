#include "Engine/Tools/SceneImport.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Debug/Console.hpp"
#include "Engine/Core/StringUtils.hpp"

//-----------------------------------------------------------------------------------------------
SceneImport::~SceneImport()
{
   for (MeshBuilder* meshBuilder : m_meshes)
   {
      delete meshBuilder;
   }
}


//-----------------------------------------------------------------------------------------------
MeshBuilder* SceneImport::CombineMeshes()
{
   MeshBuilder* masterBuilder = new MeshBuilder();

   double startTime = Time::GetCurrentTimeSeconds();
   int startSize = 0;
   int endSize = 0;
   for (MeshBuilder* currentBuilder : m_meshes)
   {
      startSize += currentBuilder->GetVertexCount();
      currentBuilder->Reduce();
      endSize += currentBuilder->GetVertexCount();
   }
   double reductionTime = Time::GetCurrentTimeSeconds() - startTime;
   g_theConsole->ConsolePrintf(Stringf("Mesh reduction time: %f", reductionTime), Rgba::GREEN);

   g_theConsole->ConsolePrintf(Stringf("Mesh Start Verts: %d", startSize), Rgba::GREEN);
   g_theConsole->ConsolePrintf(Stringf("Mesh Reduced Verts: %d", endSize), Rgba::GREEN);

   for (MeshBuilder* currentBuilder : m_meshes)
   {
      masterBuilder->AppendMeshBuilder(currentBuilder);
   }

   //masterBuilder->Reduce();

   
   return masterBuilder;
}