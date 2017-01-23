#pragma once

#include <vector>
#include "Engine/Math/Matrix4x4.hpp"


// #TODO - template
//-----------------------------------------------------------------------------------------------
class MatrixStack
{
public:
   MatrixStack();

   bool IsEmpty();
   Matrix4x4 GetTop() const;
   void Push(const Matrix4x4& mat);
   void Pop();

private:
   std::vector<Matrix4x4> m_matrices;
};