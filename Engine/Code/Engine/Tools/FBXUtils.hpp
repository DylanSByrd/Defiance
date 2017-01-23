#pragma once
#include "Engine/Debug/ConsoleCommandArgs.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#if !defined(__TOOLS_FBX__)
#define __TOOLS_FBX__


//-----------------------------------------------------------------------------------------------
class SceneImport;


//-----------------------------------------------------------------------------------------------
void FbxListScene(const std::string& filename);
void ListFBXSceneFromFile(ConsoleCommandArgs& fileArgs);
SceneImport* FbxLoadSceneFromFile(const std::string& fbxFilename, const Matrix4x4& engineBasis, bool isEngineBasisRightHanded, const Matrix4x4& transform = Matrix4x4::IDENTITY);
void ListAvailableFBXFiles(ConsoleCommandArgs&);

#endif
void RegisterFBXCommands();