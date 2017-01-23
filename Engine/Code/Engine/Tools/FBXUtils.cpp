#include "Engine/Tools/FBXUtils.hpp"

#if defined(TOOLS_BUILD)
// For TOOLS only
#include <fbxsdk.h>
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Tools/SceneImport.hpp"
#include "Engine/Debug/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MatrixStack.hpp"
#include "Engine/Math/Vector4i.hpp"
#include "Engine/Math/Vector4f.hpp"
#include "Engine/IO/FileUtils.hpp"
#pragma comment(lib, "libfbxsdk-md.lib")


//-----------------------------------------------------------------------------------------------
const int INVALID_JOINT_INDEX = -1;


//-----------------------------------------------------------------------------------------------
struct SkinWeight
{
   Vector4i indices;
   Vector4f weights;
};


//-----------------------------------------------------------------------------------------------
static bool HasSkinWeights(const FbxMesh* mesh)
{
   int deformerCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
   return (deformerCount > 0);
}


//-----------------------------------------------------------------------------------------------
static int GetLowestWeightedIndex(SkinWeight* skinWeight)
{
   int currentLowest = 0;

   for (int weightIndex = 1; weightIndex < 4; ++weightIndex)
   {
      if (skinWeight->weights[currentLowest] > skinWeight->weights[weightIndex])
      {
         currentLowest = weightIndex;
      }
   }

   return currentLowest;
}


//-----------------------------------------------------------------------------------------------
static void AddHighestWeight(SkinWeight* skinWeight, int jointIndex, float weight)
{
   int lowestIndex = GetLowestWeightedIndex(skinWeight);

   if (skinWeight->weights[lowestIndex] < weight)
   {
      skinWeight->indices[lowestIndex] = jointIndex;
      skinWeight->weights[lowestIndex] = weight;
   }
}


//-----------------------------------------------------------------------------------------------
static int GetJointIndexForNode(std::map<int, FbxNode*>& nodeMap, const FbxNode* node)
{
   std::map<int, FbxNode*>::iterator nodeIter = nodeMap.begin();
   for (nodeIter; nodeIter != nodeMap.end(); ++nodeIter)
   {
      FbxNode* currentNode = nodeIter->second;
      if (currentNode == node)
      {
         return nodeIter->first;
      }
   }

   return INVALID_JOINT_INDEX;
}


//-----------------------------------------------------------------------------------------------
static void GetSkinWeights(std::vector<SkinWeight>& skinWeights, const FbxMesh* mesh, std::map<int, FbxNode*>& nodeMap)
{
   for (size_t pointIndex = 0; pointIndex < skinWeights.size(); ++pointIndex)
   {
      skinWeights[pointIndex].indices = Vector4i(0, 0, 0, 0);
      skinWeights[pointIndex].weights = Vector4f(0.f, 0.f, 0.f, 0.f);
   }

   int deformerCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
   ASSERT_OR_DIE(deformerCount == 1, "ERROR: Engine does not support a deformerCount other than 1!!");
   for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
   {
      FbxSkin* skin = (FbxSkin*)mesh->GetDeformer(deformerIndex, FbxDeformer::eSkin);

      if (skin == nullptr)
      {
         continue;
      }

      // Clusters are a link between this skin objects, bones, and the verts that bone affects
      int clusterCount = skin->GetClusterCount();
      for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
      {
         FbxCluster* cluster = skin->GetCluster(clusterIndex);
         const FbxNode* linkNode = cluster->GetLink();

         // ignore if not associated with bone
         if (nullptr == linkNode)
         {
            continue;
         }

         int jointIndex = GetJointIndexForNode(nodeMap, linkNode);
         if (jointIndex == INVALID_JOINT_INDEX)
         {
            continue;
         }

         int* controlPointIndices = cluster->GetControlPointIndices();
         int indexCount = cluster->GetControlPointIndicesCount();
         double* weights = cluster->GetControlPointWeights();

         for (int controlPointIndex = 0; controlPointIndex < indexCount; ++controlPointIndex)
         {
            int controlIndex = controlPointIndices[controlPointIndex];
            double weight = weights[controlPointIndex];

            SkinWeight* skinWeight = &skinWeights[controlIndex];
            AddHighestWeight(skinWeight, jointIndex, (float)weight);
         }
      }
   }
}

//-----------------------------------------------------------------------------------------------
static Vector4f FbxVecToEngineVector4(const FbxDouble4& fbxVec4)
{
   return Vector4f((float)fbxVec4.mData[0], (float)fbxVec4.mData[1], (float)fbxVec4.mData[2], (float)fbxVec4.mData[3]);
}


//-----------------------------------------------------------------------------------------------
static Vector3f FbxVecToEngineVector3(const FbxVector4& fbxVec4)
{
   return Vector3f((float)fbxVec4.mData[0], (float)fbxVec4.mData[1], (float)fbxVec4.mData[2]);
}


//-----------------------------------------------------------------------------------------------
static Rgba FbxColorToEngineRgba(const FbxColor& fbxColor)
{
   return Rgba((unsigned char)(fbxColor.mRed * 255), (unsigned char)(fbxColor.mGreen * 255), (unsigned char)(fbxColor.mBlue * 255), (unsigned char)(fbxColor.mAlpha));
}


//-----------------------------------------------------------------------------------------------
static Matrix4x4 FbxMatToEngineMat(const FbxMatrix& fbxMat)
{
   Matrix4x4 engineMat;
   engineMat.SetMatrixFromRows(
      FbxVecToEngineVector4(fbxMat.mData[0]),
      FbxVecToEngineVector4(fbxMat.mData[1]),
      FbxVecToEngineVector4(fbxMat.mData[2]),
      FbxVecToEngineVector4(fbxMat.mData[3]));

   return engineMat;
}



//-----------------------------------------------------------------------------------------------
static Matrix4x4 GetNodeWorldTransformAtTime(FbxNode* node, FbxTime time, const Matrix4x4& importTransform)
{
   if (node == nullptr)
   {
      return Matrix4x4::IDENTITY;
   }

   FbxMatrix fbxMat = node->EvaluateGlobalTransform(time);
   Matrix4x4 engineMatrix = FbxMatToEngineMat(fbxMat);
   return (engineMatrix * importTransform);
}


//-----------------------------------------------------------------------------------------------
static Matrix4x4 GetSceneBasis(FbxScene* scene)
{
   fbxsdk::FbxAxisSystem axisSystem = scene->GetGlobalSettings().GetAxisSystem();
   FbxAMatrix fbxMat;
   axisSystem.GetMatrix(fbxMat);

   return FbxMatToEngineMat(fbxMat);
}


//-----------------------------------------------------------------------------------------------
static Matrix4x4 GetGeometricTransform(FbxNode* node)
{
   Matrix4x4 geoTransform(Matrix4x4::IDENTITY);

   if ((nullptr != node) && (node->GetNodeAttribute() != nullptr))
   {
      const FbxVector4 geoTranslation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
      const FbxVector4 geoRotation = node->GetGeometricRotation(FbxNode::eSourcePivot);
      const FbxVector4 geoScale = node->GetGeometricScaling(FbxNode::eSourcePivot);

      FbxMatrix fbxGeoMat;
      fbxGeoMat.SetTRS(geoTranslation, geoRotation, geoScale);
      geoTransform = FbxMatToEngineMat(fbxGeoMat);
   }

   return geoTransform;
}


//-----------------------------------------------------------------------------------------------
static Matrix4x4 GetNodeTransform(FbxNode* fbxNode)
{
   FbxMatrix fbxMat = fbxNode->EvaluateLocalTransform();
   return FbxMatToEngineMat(fbxMat);
}


//-----------------------------------------------------------------------------------------------
static const char* GetAttributeTypeName(FbxNodeAttribute::EType type)
{
   switch (type) 
   { 
      case FbxNodeAttribute::eUnknown: return "unidentified";            
      case FbxNodeAttribute::eNull: return "null";            
      case FbxNodeAttribute::eMarker: return "marker";            
      case FbxNodeAttribute::eSkeleton: return "skeleton";            
      case FbxNodeAttribute::eMesh: return "mesh";            
      case FbxNodeAttribute::eNurbs: return "nurbs";            
      case FbxNodeAttribute::ePatch: return "patch";            
      case FbxNodeAttribute::eCamera: return "camera";            
      case FbxNodeAttribute::eCameraStereo: return "stereo";            
      case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";            
      case FbxNodeAttribute::eLight: return "light";            
      case FbxNodeAttribute::eOpticalReference: return "optical reference";            
      case FbxNodeAttribute::eOpticalMarker: return "marker";            
      case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";            
      case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";            
      case FbxNodeAttribute::eBoundary: return "boundary";            
      case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";            
      case FbxNodeAttribute::eShape: return "shape";            
      case FbxNodeAttribute::eLODGroup: return "lodgroup";            
      case FbxNodeAttribute::eSubDiv: return "subdiv";            
      default: return "unknown"; 
   }
}


//-----------------------------------------------------------------------------------------------
static void PrintAttribute(FbxNodeAttribute *attribute, int depth)
{
   if (nullptr == attribute)
   {
      return;
   }

   FbxNodeAttribute::EType type = attribute->GetAttributeType();

   const char* typeName = GetAttributeTypeName(type);
   const char* attributeName = attribute->GetName();

   g_theConsole->ConsolePrintf(Stringf("%*s- type='%s' name='%s'", depth, " ", typeName, attributeName), Rgba::BLACK);
}
   
   
//-----------------------------------------------------------------------------------------------
static void PrintNode(FbxNode *node, int depth)
{
   g_theConsole->ConsolePrintf(Stringf("%*sNode [%s]", depth, " ", node->GetName()), Rgba::BLACK);

   for (int i = 0; i < node->GetNodeAttributeCount(); i++) 
   { 
      PrintAttribute(node->GetNodeAttributeByIndex(i), depth);
   }

   for (int nodeIndex = 0; nodeIndex < node->GetChildCount(); ++nodeIndex)
   {
      PrintNode(node->GetChild(nodeIndex), depth + 1);
   }
}


//-----------------------------------------------------------------------------------------------
void FbxListScene(const std::string& filename)
{
   FbxManager* fbxManager = FbxManager::Create();
   if (nullptr == fbxManager)
   {
      g_theConsole->ConsolePrintf(Stringf("Could not create fbx manager\n"), Rgba::BLACK);
      return;
   }

   FbxIOSettings *ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
   fbxManager->SetIOSettings(ioSettings);

   // Create an importer
   FbxImporter *importer = FbxImporter::Create(fbxManager, "");

   bool loadSuccessful = importer->Initialize(filename.c_str(), -1, fbxManager->GetIOSettings());

   if (loadSuccessful)
   {
      // We imported the FBX
      FbxScene *scene = FbxScene::Create(fbxManager, "");
      
      bool importSuccessful = importer->Import(scene);
      if (importSuccessful)
      {
         FbxNode *root = scene->GetRootNode();
         PrintNode(root, 0);
      }

      FBX_SAFE_DESTROY(scene);
   }
   else
   {
      g_theConsole->ConsolePrintf(Stringf("Could not import scene: %s", filename.c_str()), Rgba::RED);
   }

   FBX_SAFE_DESTROY(importer);
   FBX_SAFE_DESTROY(ioSettings);
   FBX_SAFE_DESTROY(fbxManager);
}


//-----------------------------------------------------------------------------------------------
template <typename ElementType, typename VarType>
static bool GetObjectFromElement(FbxMesh* mesh, int polygonIndex, int vertexIndex, ElementType* element, VarType* outVar)
{
   if (nullptr == element)
   {
      return false;
   }

   switch (element->GetMappingMode())
   {
   case FbxGeometryElement::eByControlPoint:
   {
      int controlIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
      switch (element->GetReferenceMode())
      {
      case FbxGeometryElement::eDirect:
      {
         if (controlIndex < element->GetDirectArray().GetCount())
         {
            *outVar = element->GetDirectArray().GetAt(controlIndex);
            return true;
         }
         break;
      }

      case FbxGeometryElement::eIndexToDirect:
      {
         if (controlIndex < element->GetIndexArray().GetCount())
         {
            int index = element->GetIndexArray().GetAt(controlIndex);
            *outVar = element->GetDirectArray().GetAt(index);
            return true;
         }
         break;
      }
      }
      break;
   }

   case FbxGeometryElement::eByPolygonVertex:
   {
      int directVertexIndex = polygonIndex * 3 + vertexIndex;
      switch (element->GetReferenceMode())
      {
      case FbxGeometryElement::eDirect:
      {
         if (directVertexIndex < element->GetDirectArray().GetCount())
         {
            *outVar = element->GetDirectArray().GetAt(directVertexIndex);
            return true;
         }
         break;
      }

      case FbxGeometryElement::eIndexToDirect:
      {
         if (directVertexIndex < element->GetIndexArray().GetCount())
         {
            int index = element->GetIndexArray().GetAt(directVertexIndex);
            *outVar = element->GetDirectArray().GetAt(index);
            return true;
         }
         break;
      }
      }

      break;
   }

   default:
   {
      ERROR_AND_DIE("Undefined mapping mode.");
   }
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
static bool GetUV(Vector2f* outUV, FbxMesh* mesh, int polygonIndex, int vertexIndex, int uvIndex)
{
   FbxVector2 uv;
   FbxGeometryElementUV *uvElement = mesh->GetElementUV(uvIndex);
   if (GetObjectFromElement(mesh, polygonIndex, vertexIndex, uvElement, &uv))
   {
      *outUV = Vector2f((float)uv.mData[0], (float)uv.mData[1]);
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
static bool GetTangent(Vector3f* outTangent, const Matrix4x4& transform, FbxMesh* mesh, int polygonIndex, int vertexIndex)
{
   FbxVector4 tangent;
   FbxGeometryElementTangent *tangentElement = mesh->GetElementTangent();
   if (GetObjectFromElement(mesh, polygonIndex, vertexIndex, tangentElement, &tangent))
   {
      Vector3f engineTan = FbxVecToEngineVector3(tangent);
      *outTangent = (transform.Transform(Vector4f(engineTan, 0.f))).xyz();
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
static bool GetBitangent(Vector3f* outBitangent, const Matrix4x4& transform, FbxMesh* mesh, int polygonIndex, int vertexIndex)
{
   FbxVector4 bitangent;
   FbxGeometryElementBinormal *bittangentElement = mesh->GetElementBinormal();
   if (GetObjectFromElement(mesh, polygonIndex, vertexIndex, bittangentElement, &bitangent))
   {
      Vector3f engineBit = FbxVecToEngineVector3(bitangent);
      *outBitangent= (transform.Transform(Vector4f(engineBit, 0.f))).xyz();
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
static bool GetNormal(Vector3f* outNormal, const Matrix4x4& transform, FbxMesh* mesh, int polygonIndex, int vertexIndex)
{
   FbxVector4 normal;
   FbxGeometryElementNormal *normalElement = mesh->GetElementNormal();
   if (GetObjectFromElement(mesh, polygonIndex, vertexIndex, normalElement, &normal))
   {
      Vector3f engineNorm = FbxVecToEngineVector3(normal);
      *outNormal = (transform.Transform(Vector4f(engineNorm, 0.f))).xyz();
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
static bool GetColor(Rgba* outColor, FbxMesh* mesh, int polygonIndex, int vertexIndex)
{
   FbxColor fbxColor;
   FbxGeometryElementVertexColor *colorElement = mesh->GetElementVertexColor();
   if (GetObjectFromElement(mesh, polygonIndex, vertexIndex, colorElement, &fbxColor))
   {
      *outColor = FbxColorToEngineRgba(fbxColor);
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
static bool GetPosition(Vector3f* outPosition, const Matrix4x4& transform, FbxMesh* mesh, int polygonIndex, int vertexIndex)
{
   FbxVector4 fbxPosition;
   int controlIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
   fbxPosition = mesh->GetControlPointAt(controlIndex);

   *outPosition = transform.Transform(Vector4f(FbxVecToEngineVector3(fbxPosition), 1.f)).xyz();
   return true;
}


//-----------------------------------------------------------------------------------------------
static void ImportVertex(MeshBuilder* mb, const Matrix4x4& transform, FbxMesh* mesh, int polygonIndex, int vertexIndex, std::vector<SkinWeight>& skinWeights)
{
   Vector3f normal;
   if (GetNormal(&normal, transform, mesh, polygonIndex, vertexIndex))
   {
      normal.Normalize();
      mb->SetCurrentVertexNormal(normal);
   }

   Vector3f tangent;
   if (GetTangent(&tangent, transform, mesh, polygonIndex, vertexIndex))
   {
      tangent.Normalize();
      mb->SetCurrentVertexTangent(tangent);
   }

   Vector3f bitangent;
   if (GetBitangent(&bitangent, transform, mesh, polygonIndex, vertexIndex))
   {
      tangent.Normalize();
      mb->SetCurrentVertexBitangent(bitangent);
   }

   Vector2f uv;
   if (GetUV(&uv, mesh, polygonIndex, vertexIndex, 0))
   {
      mb->SetCurrentVertexUV0(uv);
   }

   Rgba color;
   if (GetColor(&color, mesh, polygonIndex, vertexIndex))
   {
      mb->SetCurrentVertexColor(color);
   }

   size_t controlIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
   if (controlIndex < skinWeights.size())
   {
      mb->SetBoneWeights(skinWeights[controlIndex].indices, skinWeights[controlIndex].weights);
      mb->RenormalizeSkinWeights();
   }
   else
   {
      mb->ClearBoneWeights();
   }

   Vector3f position;
   if (GetPosition(&position, transform, mesh, polygonIndex, vertexIndex))
   {
      mb->SetCurrentVertexPosition(position);
      mb->AddCurrentVertexToMesh();
   }
}


//-----------------------------------------------------------------------------------------------
static void ImportMesh(SceneImport* import, FbxMesh* mesh, MatrixStack &matStack, const std::string& materialName, std::map<int, FbxNode*>& nodeMap)
{
   MeshBuilder *mb = new MeshBuilder();

   // #TODO - some kind of assert?
   if (!mesh->IsTriangleMesh())
   {
      return;
   }

   //mb->BeginDrawInstruction(GL_TRIANGLES, true);
   mb->BeginDrawInstruction();

   Matrix4x4 geoTransform = GetGeometricTransform(mesh->GetNode());
   matStack.Push(geoTransform);
   Matrix4x4 transform = matStack.GetTop();

   int controlPointCount = mesh->GetControlPointsCount();
   std::vector<SkinWeight> skinWeights;
   skinWeights.resize(controlPointCount);
   if (HasSkinWeights(mesh))
   {
      GetSkinWeights(skinWeights, mesh, nodeMap);
   }
   else
   {
      FbxNode* node = mesh->GetNode();
      node = node->GetParent();
      while (!(node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton))
      {
         node = node->GetParent();
      }

      int jointIndex = GetJointIndexForNode(nodeMap, node);

      for (int controlPointIndex = 0; controlPointIndex < controlPointCount; ++controlPointIndex)
      {
         SkinWeight* skinWeight = &skinWeights[controlPointIndex];
         skinWeight->indices = Vector4i(jointIndex, 0, 0, 0);
         skinWeight->weights = Vector4f(1.f, 0.f, 0.f, 0.f);         
      }
   }


   // Import the mesh
   int polygonCount = mesh->GetPolygonCount();
   for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
   {
      int vertexCount = mesh->GetPolygonSize(polygonIndex);

      // must always be true
      ASSERT_OR_DIE(vertexCount == 3, "Error: mesh is not triangular!");
      for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
      {
         ImportVertex(mb, transform, mesh, polygonIndex, vertexIndex, skinWeights);
      }
   }

   mb->EndDrawInstruction();
   matStack.Pop();
   mb->SetMaterialName(materialName);
   import->AddMesh(mb); 
}


//-----------------------------------------------------------------------------------------------
static void ImportNodeAttribute(SceneImport* import, FbxNodeAttribute* attribute, MatrixStack& matStack, const std::string& materialName, std::map<int, FbxNode*>& nodeMap)
{
   if (nullptr == attribute)
   {
      return;
   }

   switch (attribute->GetAttributeType())
   {
   case FbxNodeAttribute::eMesh:
   {
      ImportMesh(import, (FbxMesh*)attribute, matStack, materialName, nodeMap);
      break;
   }

   default:
   {
      break;
   }
   }
}


//-----------------------------------------------------------------------------------------------
static void ImportSceneNode(SceneImport* import, FbxNode* node, MatrixStack& matStack, std::map<int, FbxNode*>& nodeMap)
{
   if (nullptr == node)
   {
      return;
   }

   Matrix4x4 nodeLocalTransform = GetNodeTransform(node);
   matStack.Push(nodeLocalTransform);

   // Check for materials
   int materialCount = node->GetMaterialCount();
   std::string materialName;
   if (materialCount > 0)
   {
      FbxSurfaceMaterial* fbxMaterial = node->GetMaterial(0);
      materialName = fbxMaterial->GetName();
   }
   
   // We want to load meshes
   int attributeCount = node->GetNodeAttributeCount();
   for (int attributeIndex = 0; attributeIndex < attributeCount; ++attributeIndex)
   {
      ImportNodeAttribute(import, node->GetNodeAttributeByIndex(attributeIndex), matStack, materialName, nodeMap);
   }


   // Import children
   int childCount = node->GetChildCount();
   for (int childIndex = 0; childIndex < childCount; ++childIndex)
   {
      ImportSceneNode(import, node->GetChild(childIndex), matStack, nodeMap);
   }

   matStack.Pop();
}


//-----------------------------------------------------------------------------------------------
static void TriangulateScene(FbxScene* scene)
{
   FbxGeometryConverter converter(scene->GetFbxManager());
   converter.Triangulate(scene, true);
}


//-----------------------------------------------------------------------------------------------
static Skeleton* ImportSkeleton(SceneImport* import, MatrixStack& stack, Skeleton* skeleton, int parentJointIndex, FbxSkeleton* fbxSkeleton)
{
   Skeleton* retSkeleton = nullptr;
   if (fbxSkeleton->IsSkeletonRoot())
   {
      retSkeleton = new Skeleton();
      import->AddSkeleton(retSkeleton);
   }
   else
   {
      retSkeleton = skeleton;
      ASSERT_OR_DIE(retSkeleton != nullptr, "ERROR! Return skeleton in ImportSkeleton is nullptr!");
   }

   Matrix4x4 geoTransform = GetGeometricTransform(fbxSkeleton->GetNode());
   stack.Push(geoTransform);

   Matrix4x4 modelSpace = stack.GetTop();
   retSkeleton->AddJoint(fbxSkeleton->GetNode()->GetName(), parentJointIndex, modelSpace);
   stack.Pop();

   return retSkeleton;
}


//-----------------------------------------------------------------------------------------------
static void ImportSkeletons(SceneImport* import, FbxNode* node, MatrixStack& stack, Skeleton* skeleton, int parentJointIndex, std::map<int, FbxNode*>& map)
{
   if (nullptr == node)
   {
      return;
   }

   Matrix4x4 mat = GetNodeTransform(node);
   stack.Push(mat);

   // walk attributes and look for skeletons
   int attribCount = node->GetNodeAttributeCount();
   for (int attribIndex = 0; attribIndex < attribCount; ++attribIndex)
   {
      FbxNodeAttribute* attrib = node->GetNodeAttributeByIndex(attribIndex);
      if ((attrib != nullptr)
         && (attrib->GetAttributeType() == FbxNodeAttribute::eSkeleton))
      {
         FbxSkeleton* fbxSkeleton = (FbxSkeleton*) attrib;
         Skeleton* newSkeleton = ImportSkeleton(import, stack, skeleton, parentJointIndex, fbxSkeleton);

         if (newSkeleton != nullptr)
         {
            skeleton = newSkeleton;
            parentJointIndex = skeleton->GetLastAddedJointIndex();
            map[parentJointIndex] = node;
         }
      }
   }

   int childCount = node->GetChildCount();
   for (int childIndex = 0; childIndex < childCount; ++childIndex)
   {
      ImportSkeletons(import, node->GetChild(childIndex), stack, skeleton, parentJointIndex, map);
   }

   stack.Pop();
}


//-----------------------------------------------------------------------------------------------
static void ImportMotions(SceneImport* import, const Matrix4x4& importTransform, std::map<int, FbxNode*>& map, FbxScene* scene, float framerate)
{
   int animationCount = scene->GetSrcObjectCount<FbxAnimStack>();
   if (0 == animationCount)
   {
      return;
   }

   if (0 == import->GetSkeletonCount())
   {
      return;
   }

   FbxGlobalSettings& settings = scene->GetGlobalSettings();
   FbxTime::EMode timeMode = settings.GetTimeMode();
   double sceneFrameRate;
   if (timeMode == FbxTime::eCustom)
   {
      sceneFrameRate = settings.GetCustomFrameRate();
   }
   else
   {
      sceneFrameRate = FbxTime::GetFrameRate(timeMode);
   }

   //uint32_t skeletonCount = import->GetSkeletonCount();
   Skeleton* skeleton = import->GetSkeleton(0);

   FbxTime advance;
   advance.SetSecondDouble((double)(1.f / framerate));

   for (int animationIndex = 0; animationIndex < animationCount; ++animationIndex)
   {
      FbxAnimStack* anim = scene->GetSrcObject<FbxAnimStack>();
      if (nullptr == anim)
      {
         continue;
      }

      // startTime should be adjusted to 0
      FbxTime startTime = anim->LocalStart;
      FbxTime endTime = anim->LocalStop;
      FbxTime duration = endTime - startTime;

      scene->SetCurrentAnimationStack(anim);

      std::string motionName = anim->GetName();
      float timeSpan = (float)duration.GetSecondDouble();

      Motion* motion = new Motion(motionName, Motion::CLAMP, timeSpan, framerate, skeleton);

      int jointCount = skeleton->GetJointCount();
      for (int JointIndex = 0; JointIndex < jointCount; ++JointIndex)
      {
         FbxNode* node = map[JointIndex]; // #TODO - don't use [] because it makes the entry
         // also, currently assuming nodeIter != end

         Matrix4x4* boneKeyframes = motion->GetJointKeyFrames(JointIndex);

         FbxTime evalTime = FbxTime(0);
         for (uint32_t frameIndex = 0; frameIndex < motion->GetFrameCount(); ++frameIndex)
         {
            Matrix4x4* boneKeyframe = boneKeyframes + frameIndex;
            Matrix4x4 boneTransform = GetNodeWorldTransformAtTime(node, evalTime, importTransform);
            *boneKeyframe = boneTransform;
            
            evalTime += advance;
         }
      }

      import->AddMotion(motion);
   }

}


//-----------------------------------------------------------------------------------------------
static void ImportScene(SceneImport* import, FbxScene* scene, MatrixStack& matStack)
{
   TriangulateScene(scene);

   FbxNode* root = scene->GetRootNode();


   std::map<int, FbxNode*> nodeToJoint;
   ImportSkeletons(import, root, matStack, nullptr, -1, nodeToJoint);

   ImportSceneNode(import, root, matStack, nodeToJoint);

   const float FRAME_RATE = 30.f;
   ImportMotions(import, matStack.GetTop(), nodeToJoint, scene, FRAME_RATE);
}


//-----------------------------------------------------------------------------------------------
SceneImport* FbxLoadSceneFromFile(const std::string& fbxFilename, const Matrix4x4& engineBasis, bool isEngineBasisRightHanded, const Matrix4x4& transform /*== identity/scale*/)
{
   SceneImport* import = nullptr;
   FbxManager* fbxManager = FbxManager::Create();
   if (nullptr == fbxManager)
   {
      g_theConsole->ConsolePrintf(Stringf("Could not create fbx manager\n"), Rgba::BLACK);
      return import;
   }

   FbxIOSettings *ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
   fbxManager->SetIOSettings(ioSettings);

   // Create an importer
   FbxImporter *importer = FbxImporter::Create(fbxManager, "");

   bool loadSuccessful = importer->Initialize(fbxFilename.c_str(), -1, fbxManager->GetIOSettings());

   if (loadSuccessful)
   {
      // We imported the FBX
      FbxScene *scene = FbxScene::Create(fbxManager, "");

      bool importSuccessful = importer->Import(scene);
      if (importSuccessful)
      {
         import = new SceneImport;
         MatrixStack matStack;
         matStack.Push(transform);
         matStack.Push(engineBasis);

         Matrix4x4 sceneBasis = GetSceneBasis(scene);
         sceneBasis.Transpose();

         if (!isEngineBasisRightHanded)
         {
            Vector3f forward = sceneBasis.GetForward();
            sceneBasis.SetForward(-forward);
         }

         matStack.Push(sceneBasis);
         ImportScene(import, scene, matStack);
      }

      FBX_SAFE_DESTROY(scene);
   }
   else
   {
      g_theConsole->ConsolePrintf(Stringf("Could not import scene: %s", fbxFilename.c_str()), Rgba::RED);
   }

   FBX_SAFE_DESTROY(importer);
   FBX_SAFE_DESTROY(ioSettings);
   FBX_SAFE_DESTROY(fbxManager);

   return import;
}


//-----------------------------------------------------------------------------------------------
void RegisterFBXCommands()
{
   g_theConsole->RegisterCommand("fbx_list_scene", "Takes in the name of an fbx file and prints a list of scene nodes", ListFBXSceneFromFile);
   g_theConsole->RegisterCommand("fbx_list_all_files", "Prints all available FBX files to the console", ListAvailableFBXFiles);
}


//-----------------------------------------------------------------------------------------------
void ListFBXSceneFromFile(ConsoleCommandArgs& fileArgs)
{
   std::string name;
   bool success = fileArgs.GetNextArgAsString(&name, "");
   name = ToLowerCase(name);
   name = Stringf("Data/Models/%s.fbx", name.c_str());
   if (success)
   {
      FbxListScene(name);
   }
   else
   {
      g_theConsole->ConsolePrintf("FBX file name required.", Rgba::RED);
   }
}


//-----------------------------------------------------------------------------------------------
void ListAvailableFBXFiles(ConsoleCommandArgs&)
{
   std::vector<std::string> fbxFileNames = EnumerateFilesInDirectory("Data/Models", "*.fbx");

   if (fbxFileNames.size() == 0)
   {
      g_theConsole->ConsolePrintf("No FBX files found.", Rgba::RED);
      return;
   }

   g_theConsole->ConsolePrintf("Listing FBX files:", Rgba::GREEN);
   for (const std::string& fbxFile : fbxFileNames)
   {
      g_theConsole->ConsolePrintf(fbxFile, Rgba::GREEN);
   }
}

   
#else // !defined(TOOLS_BUILD)
#include <string>
void RegisterFBXCommands() {}
void FbxListScene(const std::string&) {}
SceneImport* FbxLoadSceneFromFile(const std::string&, const Matrix4x4&, bool, const Matrix4x4&) { return nullptr; }
void ListAvailableFBXFiles(ConsoleCommandArgs&) {}
#endif