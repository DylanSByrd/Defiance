#include <cstring>
#include <cmath>
#include <map>
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector3f.hpp"
#include "Engine/Math/Vector4f.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const Matrix4x4 Matrix4x4::IDENTITY = Matrix4x4
(
   1.f, 0.f, 0.f, 0.f,
   0.f, 1.f, 0.f, 0.f,
   0.f, 0.f, 1.f, 0.f,
   0.f, 0.f, 0.f, 1.f
);


//-----------------------------------------------------------------------------------------------
Matrix4x4::Matrix4x4(const Vector4f& row0, const Vector4f& row1, const Vector4f& row2, const Vector4f& row3)
   : data() 
{
   SetMatrixFromRows(row0, row1, row2, row3);
}


//-----------------------------------------------------------------------------------------------
Matrix4x4::Matrix4x4(float r11, float r21, float r31, float r41, float r12, float r22, float r32, float r42, float r13, float r23, float r33, float r43, float r14, float r24, float r34, float r44)
{
   data[0] = r11;
   data[1] = r21;
   data[2] = r31;
   data[3] = r41;

   data[4] = r12;
   data[5] = r22;
   data[6] = r32;
   data[7] = r42;

   data[8] = r13;
   data[9] = r23;
   data[10] = r33;
   data[11] = r43;

   data[12] = r14;
   data[13] = r24;
   data[14] = r34;
   data[15] = r44;
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::MakeIdentity()
{
   const float identity[] = 
   {
      1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      0.f, 0.f, 0.f, 1.f,
   };

   memcpy(data, identity, sizeof(data));
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::MakeProjOrthogonal(float width, float height, float nearZ, float farZ)
{
   float sz = 1.f / (farZ - nearZ);

   const float vals[] =
   {
      2.f / width, 0.f, 0.f, 0.f,
      0.f, 2.f / height, 0.f, 0.f,
      0.f, 0.f, 2.f * sz, 0.f,
      0.f, 0.f,  -(farZ + nearZ) * sz, 1.f,
   };

   memcpy(data, vals, sizeof(vals));
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::MakePerspective(float fieldOfViewDegreesY, float aspect, float near, float far)
{
   float fieldOfViewRadiansY = DegreesToRadians(fieldOfViewDegreesY);
   float viewSize = 1.f / tan(fieldOfViewRadiansY / 2.f);

   float width = viewSize;
   float height = viewSize;
   if (aspect > 1.f)
   {
      width /= aspect;
   }
   else
   {
      height *= aspect;
   }

   float temp = 1.f / (far - near);

   const float values[] =
   {
      width, 0.f, 0.f, 0.f,
      0.f, height, 0.f, 0.f,
      0.f, 0.f, (far + near) * temp, 1.f,
      0.f, 0.f, -2.f * near * far * temp, 0.f,
   };

   memcpy(data, values, sizeof(values));
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::MakeTranslation(float x, float y, float z)
{
   const float values[] =
   {
      1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      x, y, z, 1.f,
   };

   memcpy(data, values, sizeof(values));
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::MakeTranslation(const Vector3f& position)
{
   MakeTranslation(position.x, position.y, position.z);
}


//-----------------------------------------------------------------------------------------------
Vector3f Matrix4x4::Transform(const Vector3f& vec) const
{
   Vector4f newVec(vec.x, vec.y, vec.z, 1.f);
   Vector4f result = Transform(newVec);

   return Vector3f(result.x, result.y, result.x);
}


//-----------------------------------------------------------------------------------------------
Vector4f Matrix4x4::Transform(const Vector4f& vec) const
{
   Vector4f result = Vector4f(
      vec.DotProduct(GetCol(0)),
      vec.DotProduct(GetCol(1)),
      vec.DotProduct(GetCol(2)),
      vec.DotProduct(GetCol(3)));

   return result;
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::MakeLookAt(const Vector3f& currentPos, const Vector3f& worldUpDir, const Vector3f& targetPos)
{
   Vector3f forward = targetPos - currentPos;
   forward.Normalize();

   Vector3f right = worldUpDir.CrossProduct(forward);
   right.Normalize();

   Vector3f up = right.CrossProduct(forward);

   const float values[] =
   {
      right.x, right.y, right.z, 0.f,
      up.x, up.y, up.z, 0.f,
      forward.x, forward.y, forward.z, 0.f,
      currentPos.x, currentPos.y, currentPos.z, 1.f,
   };
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::Transpose()
{
   std::swap(data[1], data[4]);
   std::swap(data[2], data[8]);
   std::swap(data[3], data[12]);
   std::swap(data[6], data[9]);
   std::swap(data[7], data[13]);
   std::swap(data[11], data[14]);
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::InvertOrthonormal()
{
  const Vector3f& offset = GetOffset();
  Vector3f newOffset = -offset;

  Matrix4x4 rotInverse(*this);

  std::swap(rotInverse.data[1], rotInverse.data[4]);
  std::swap(rotInverse.data[2], rotInverse.data[8]);
  std::swap(rotInverse.data[6], rotInverse.data[9]);

  rotInverse.data[12] = 0.f;
  rotInverse.data[13] = 0.f;
  rotInverse.data[14] = 0.f;
  rotInverse.data[15] = 1.f;

  Matrix4x4 tInverse;
  tInverse.MakeIdentity();
  tInverse.SetOffset(newOffset);

  (*this) = tInverse * rotInverse;
}


//-----------------------------------------------------------------------------------------------
Matrix4x4 Matrix4x4::GetInverseOrthonormal() const
{
   Matrix4x4 inverse(*this);
   inverse.InvertOrthonormal();
   return inverse;
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::SetOffset(const Vector3f& offset)
{
   data[12] = offset.x;
   data[13] = offset.y;
   data[14] = offset.z;
}


//-----------------------------------------------------------------------------------------------
Vector3f Matrix4x4::GetOffset() const
{
   return Vector3f(data[12], data[13], data[14]);
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::MakeRotationEuler(float yaw, float pitch, float roll, const Vector3f& offset)
{
   float sinX = SinDegrees(pitch);
   float cosX = CosDegrees(pitch);

   float sinY = SinDegrees(yaw);
   float cosY = CosDegrees(yaw);

   float sinZ = SinDegrees(roll);
   float cosZ = CosDegrees(roll);

   float const rotVals[] =
   {
      cosY * cosZ + sinX * sinY * sinZ,   -cosX * sinZ,   -cosZ * sinY + cosY * sinX * sinZ,   0.f,
      cosY * sinZ - cosZ * sinX * sinY,   cosX * cosZ,    -sinY * sinZ - cosY * cosZ * sinX,   0.f,
      cosX * sinY,                        sinX,           cosX * cosY,                         0.f,
      0.f,                                0.f,            0.f,                                 1.f,
   };

   float const offsetVals[] =
   {
      1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      offset.x, offset.y, offset.z, 1.f,
   };

   Matrix4x4 rotMat;
   Matrix4x4 offsetMat;

   memcpy(rotMat.data, rotVals, sizeof(rotVals));
   memcpy(offsetMat.data, offsetVals, sizeof(offsetVals));

   (*this) = rotMat * offsetMat;
}


//-----------------------------------------------------------------------------------------------
Vector4f Matrix4x4::GetRow(int rowIndex) const
{
   switch (rowIndex)
   {
   case 0:
   {
      return Vector4f(data[0], data[1], data[2], data[3]);
   }

   case 1:
   {
      return Vector4f(data[4], data[5], data[6], data[7]);
   }

   case 2:
   {
      return Vector4f(data[8], data[9], data[10], data[11]);
   }

   case 3:
   {
      return Vector4f(data[12], data[13], data[14], data[15]);
   }
   }

   // #TODO - handle better
   return Vector4f(0.f, 0.f, 0.f, 0.f);
}


//-----------------------------------------------------------------------------------------------
Vector4f Matrix4x4::GetCol(int colIndex) const
{
   switch (colIndex)
   {
   case 0:
   {
      return Vector4f(data[0], data[4], data[8], data[12]);
   }

   case 1:
   {
      return Vector4f(data[1], data[5], data[9], data[13]);
   }

   case 2:
   {
      return Vector4f(data[2], data[6], data[10], data[14]);
   }

   case 3:
   {
      return Vector4f(data[3], data[7], data[11], data[15]);
   }
   }

   // #TODO - handle better
   return Vector4f(0.f, 0.f, 0.f, 0.f);
}


//-----------------------------------------------------------------------------------------------
Vector3f Matrix4x4::GetRight() const
{
   return GetRow(0).xyz();
}


//-----------------------------------------------------------------------------------------------
Vector3f Matrix4x4::GetUp() const
{
   return GetRow(1).xyz();
}


//-----------------------------------------------------------------------------------------------
Vector3f Matrix4x4::GetForward() const
{
   return GetRow(2).xyz();
}


//-----------------------------------------------------------------------------------------------
Vector3f Matrix4x4::GetTranslation() const
{
   return GetRow(3).xyz();
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::SetRow(int rowIndex, const Vector4f& newRow)
{
   switch (rowIndex)
   {
   case 0:
   {
      data[0] = newRow.x;
      data[1] = newRow.y;
      data[2] = newRow.z;
      data[3] = newRow.w;
   }

   case 1:
   {
      data[4] = newRow.x;
      data[5] = newRow.y;
      data[6] = newRow.z;
      data[7] = newRow.w;
   }

   case 2:
   {
      data[8] = newRow.x;
      data[9] = newRow.y;
      data[10] = newRow.z;
      data[11] = newRow.w;
   }

   case 3:
   {
      data[12] = newRow.x;
      data[13] = newRow.y;
      data[14] = newRow.z;
      data[15] = newRow.w;
   }
   }
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::SetForward(const Vector3f& newForward)
{
   data[8] = newForward.x;
   data[9] = newForward.y;
   data[10] = newForward.z;
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::SetMatrixFromRows(const Vector4f& row0, const Vector4f& row1, const Vector4f& row2, const Vector4f& row3)
{
   // row0
   data[0] = row0.x;
   data[1] = row0.y;
   data[2] = row0.z;
   data[3] = row0.w;

   // row1
   data[4] = row1.x;
   data[5] = row1.y;
   data[6] = row1.z;
   data[7] = row1.w;

   // row2
   data[8] = row2.x;
   data[9] = row2.y;
   data[10] = row2.z;
   data[11] = row2.w;

   // row3
   data[12] = row3.x;
   data[13] = row3.y;
   data[14] = row3.z;
   data[15] = row3.w;
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::SetMatrixFromCols(const Vector4f& col0, const Vector4f& col1, const Vector4f& col2, const Vector4f& col3)
{
   // col0
   data[0] = col0.x;
   data[4] = col0.y;
   data[8] = col0.z;
   data[12] = col0.w;

   // col1
   data[1] = col1.x;
   data[5] = col1.y;
   data[9] = col1.z;
   data[13] = col1.w;

   // col2
   data[2] = col2.x;
   data[6] = col2.y;
   data[10] = col2.z;
   data[14] = col2.w;

   // col3
   data[3] = col3.x;
   data[7] = col3.y;
   data[11] = col3.z;
   data[15] = col3.w;
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::SetUniformScale(float scale)
{
   data[0] = scale;
   data[5] = scale;
   data[10] = scale;
}


//-----------------------------------------------------------------------------------------------
Matrix4x4 Matrix4x4::GetInverse() const
{
   ERROR_AND_DIE("ERROR: Matrix4x4 GetInverse() not implemented!");
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::GetBasis(Vector3f* outRight, Vector3f* outUp, Vector3f* outForward, Vector3f* outTranslation) const
{
   *outRight = GetRight();
   *outUp = GetUp();
   *outForward = GetForward();
   *outTranslation = GetTranslation();
}


//-----------------------------------------------------------------------------------------------
STATIC Matrix4x4 Matrix4x4::MakeFromBasis(const Vector3f& right, const Vector3f& up, const Vector3f& forward, const Vector3f& translation)
{
   Matrix4x4 newMat;
   newMat.SetRow(0, Vector4f(right, 0.f));
   newMat.SetRow(1, Vector4f(up, 0.f));
   newMat.SetRow(2, Vector4f(forward, 0.f));
   newMat.SetRow(3, Vector4f(translation, 1.f));

   return newMat;
}


//-----------------------------------------------------------------------------------------------
STATIC Matrix4x4 Matrix4x4::MatLerp(const Matrix4x4& start, const Matrix4x4& end, float time)
{
   Vector3f rightStart;
   Vector3f upStart;
   Vector3f forwardStart;
   Vector3f translationStart;
   start.GetBasis(&rightStart, &upStart, &forwardStart, &translationStart);

   Vector3f rightEnd;
   Vector3f upEnd;
   Vector3f forwardEnd;
   Vector3f translationEnd;
   end.GetBasis(&rightEnd, &upEnd, &forwardEnd, &translationEnd);

   Vector3f lerpRight;
   Vector3f lerpUp;
   Vector3f lerpForward;
   Vector3f lerpTranslation;
   lerpRight = Slerp(rightStart, rightEnd, time);
   lerpUp = Slerp(upStart, upEnd, time);
   lerpForward = Slerp(forwardStart, forwardEnd, time);
   lerpTranslation = Lerp(translationStart, translationEnd, time);

   Matrix4x4 returnMat = MakeFromBasis(lerpRight, lerpUp, lerpForward, lerpTranslation);
   return returnMat;
}

//-----------------------------------------------------------------------------------------------
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rhs) const
{
   Matrix4x4 result;
   for (int rowIndex = 0; rowIndex < 4; ++rowIndex)
   {
      Vector4f row = GetRow(rowIndex);
      for (int colIndex = 0; colIndex < 4; ++colIndex)
      {
         result.data[(rowIndex * 4) + colIndex] = row.DotProduct(rhs.GetCol(colIndex));
      }
   }

   return result;
}


//-----------------------------------------------------------------------------------------------
const Matrix4x4& Matrix4x4::operator=(const Matrix4x4& rhs)
{
   memcpy(data, rhs.data, sizeof(rhs.data));
   return *this;
}


//-----------------------------------------------------------------------------------------------
bool Matrix4x4::operator==(const Matrix4x4& rhs) const
{
   bool isEqual = true;

   if (data[0] == rhs.data[0]
      && data[1] == rhs.data[1]
      && data[2] == rhs.data[2]
      && data[3] == rhs.data[3]
      && data[4] == rhs.data[4]
      && data[5] == rhs.data[5]
      && data[6] == rhs.data[6]
      && data[7] == rhs.data[7]
      && data[8] == rhs.data[8]
      && data[9] == rhs.data[9]
      && data[10] == rhs.data[10]
      && data[11] == rhs.data[11]
      && data[12] == rhs.data[12]
      && data[13] == rhs.data[13]
      && data[14] == rhs.data[14]
      && data[15] == rhs.data[15])
   {
      return isEqual;
   }

   return !isEqual;
}