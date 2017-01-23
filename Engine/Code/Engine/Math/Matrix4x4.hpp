#pragma once


//-----------------------------------------------------------------------------------------------
class Vector3f;
class Vector4f;


//-----------------------------------------------------------------------------------------------
// Row Major
class Matrix4x4
{
public:
   Matrix4x4() {}
   Matrix4x4(const Matrix4x4& mat) : data() { memcpy(data, mat.data, sizeof(mat.data)); }
   Matrix4x4(const Vector4f& row0, const Vector4f& row1, const Vector4f& row2, const Vector4f& row3);
   Matrix4x4
   (
      float r11, float r21, float r31, float r41,
      float r12, float r22, float r32, float r42,
      float r13, float r23, float r33, float r43,
      float r14, float r24, float r34, float r44
   );
   
   void MakeIdentity();
   void MakeProjOrthogonal(float width, float height, float nearZ, float farZ);
   void MakePerspective(float fieldOfViewDegreesY, float aspect, float near, float far);
   void MakeTranslation(float x, float y, float z);
   void MakeTranslation(const Vector3f& position);
   Vector3f Transform(const Vector3f& vec) const;
   Vector4f Transform(const Vector4f& vec) const;
   void MakeLookAt(const Vector3f& currentPos, const Vector3f& worldUpDir, const Vector3f& targetPos);
   void Transpose();
   void InvertOrthonormal();
   Matrix4x4 GetInverseOrthonormal() const;
   void SetOffset(const Vector3f& offset);
   Vector3f GetOffset() const;
   void MakeRotationEuler(float yaw, float pitch, float roll, const Vector3f& offset);
   Vector4f GetRow(int rowIndex) const;
   Vector4f GetCol(int colIndex) const;
   Vector3f GetRight() const;
   Vector3f GetUp() const;
   Vector3f GetForward() const;
   Vector3f GetTranslation() const;

   void SetRow(int rowIndex, const Vector4f& newRow);
   void SetForward(const Vector3f& newForward);
   void SetMatrixFromRows(const Vector4f& row0, const Vector4f& row1, const Vector4f& row2, const Vector4f& row3);
   void SetMatrixFromCols(const Vector4f& col0, const Vector4f& col1, const Vector4f& col2, const Vector4f& col3);
   void SetUniformScale(float scale);
   Matrix4x4 GetInverse() const;
   void GetBasis(Vector3f* outRight, Vector3f* outUp, Vector3f* outForward, Vector3f* outTranslation) const;

   static Matrix4x4 MakeFromBasis(const Vector3f& right, const Vector3f& up, const Vector3f& forward, const Vector3f& translation);
   static Matrix4x4 MatLerp(const Matrix4x4& start, const Matrix4x4& end, float time);

   Matrix4x4 operator*(const Matrix4x4& rhs) const;
   const Matrix4x4& operator= (const Matrix4x4& rhs);
   bool operator==(const Matrix4x4& rhs) const;
   bool operator!=(const Matrix4x4& rhs) const { return !operator==(rhs); }

   static const Matrix4x4 IDENTITY;

   float data[16];
};