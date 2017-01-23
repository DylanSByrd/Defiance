#include "Engine/Math/MatrixStack.hpp"


//-----------------------------------------------------------------------------------------------
MatrixStack::MatrixStack()
{
   m_matrices.push_back(Matrix4x4::IDENTITY);
}


//-----------------------------------------------------------------------------------------------
bool MatrixStack::IsEmpty()
{
   return (m_matrices.size() == 1);
}


//-----------------------------------------------------------------------------------------------
Matrix4x4 MatrixStack::GetTop() const
{
   return m_matrices.back();
}


//-----------------------------------------------------------------------------------------------
void MatrixStack::Push(const Matrix4x4& mat)
{
   Matrix4x4 top = GetTop();
   Matrix4x4 newTop = mat * top;
   m_matrices.push_back(newTop);
}


//-----------------------------------------------------------------------------------------------
void MatrixStack::Pop()
{
   if (!IsEmpty())
   {
      m_matrices.pop_back();
   }
}
