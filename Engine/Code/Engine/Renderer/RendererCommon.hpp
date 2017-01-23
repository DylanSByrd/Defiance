#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector3f.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Math/Vector2f.hpp"
#include "Engine/Math/Vector4i.hpp"
#include "Engine/Math/Vector4f.hpp"


//-----------------------------------------------------------------------------------------------
typedef unsigned int GLuint;
typedef int GLint;
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef int GLsizei;
typedef GLuint ShaderID;
typedef void GLvoid;


//-----------------------------------------------------------------------------------------------
struct Vertex_PCT
{
   Vertex_PCT(const Vector3f& pos, const Rgba& color, const TexCoords& tCoords);
   Vertex_PCT(const Vector3f& pos, const Rgba& color);
   Vertex_PCT(const Vector3f& pos);

   Vector3f position;
   Rgba color;
   TexCoords texCoords;

   Vertex_PCT();
};

inline Vertex_PCT::Vertex_PCT()
   : position(Vector3f::ZERO)
   , color(Rgba::WHITE)
   , texCoords(TexCoords::ZERO)
{}

inline Vertex_PCT::Vertex_PCT(const Vector3f& pos, const Rgba& col, const TexCoords& tCoords)
   : position(pos)
   , color(col)
   , texCoords(tCoords)
{}

inline Vertex_PCT::Vertex_PCT(const Vector3f& pos)
   : position(pos)
   , color(Rgba::WHITE)
   , texCoords(TexCoords::ZERO)
{}

inline Vertex_PCT::Vertex_PCT(const Vector3f& pos, const Rgba& col)
   : position(pos)
   , color(col)
   , texCoords(TexCoords::ZERO)
{}


//-----------------------------------------------------------------------------------------------
struct Vertex_PCUTB
{
   Vertex_PCUTB(const Vector3f& pos, const Rgba& col, const TexCoords& coords, const Vector3f& tan, const Vector3f& bit);

   Vector3f position;
   Rgba color;
   TexCoords uv;
   Vector3f tangent;
   Vector3f bitangent;
};

inline Vertex_PCUTB::Vertex_PCUTB(const Vector3f& pos, const Rgba& col, const TexCoords& coords, const Vector3f& tan, const Vector3f& bit)
   : position(pos)
   , color(col)
   , uv(coords)
   , tangent(tan)
   , bitangent(bit)
{}


//-----------------------------------------------------------------------------------------------
struct Vertex_PCUTBN
{
   Vertex_PCUTBN(const Vector3f& pos, const Rgba& col, const TexCoords& coords, const Vector3f& tan, const Vector3f& bit, const Vector3f& norm);

   Vector3f position;
   Rgba color;
   TexCoords uv;
   Vector3f tangent;
   Vector3f bitangent;
   Vector3f normal;
};

inline Vertex_PCUTBN::Vertex_PCUTBN(const Vector3f& pos, const Rgba& col, const TexCoords& coords, const Vector3f& tan, const Vector3f& bit, const Vector3f& norm)
   : position(pos)
   , color(col)
   , uv(coords)
   , tangent(tan)
   , bitangent(bit)
   , normal(norm)
{}


//-----------------------------------------------------------------------------------------------
struct Vertex_Bone
{
   Vertex_Bone(const Vector3f& pos, const Rgba& col, const TexCoords& coords, const Vector3f& tan, const Vector3f& bit, const Vector3f& norm, const Vector4i& indices, const Vector4f& weights);

   Vector3f position;
   Rgba color;
   TexCoords uv;
   Vector3f tangent;
   Vector3f bitangent;
   Vector3f normal;
   Vector4i boneIndices;
   Vector4f boneWeights;
};

inline Vertex_Bone::Vertex_Bone(const Vector3f& pos, const Rgba& col, const TexCoords& coords, const Vector3f& tan, const Vector3f& bit, const Vector3f& norm, const Vector4i& indices, const Vector4f& weights)
   : position(pos)
   , color(col)
   , uv(coords)
   , tangent(tan)
   , bitangent(bit)
   , normal(norm)
   , boneIndices(indices)
   , boneWeights(weights)
{}


// Struct holding every possible piece of data a vertex definition could want
//-----------------------------------------------------------------------------------------------
struct VertexMaster
{
   VertexMaster();

   bool operator==(const VertexMaster& rhs);

   Vector3f position;
   Vector3f tangent;
   Vector3f bitangent;
   Vector3f normal;
   Rgba color;

   // #TODO - array size MAX_UVS
   TexCoords uv0;
   TexCoords uv1;

   Vector4f boneWeights;
   Vector4i boneIndices;
};

inline VertexMaster::VertexMaster()
   : position(Vector3f::ZERO)
   , tangent(Vector3f::ZERO)
   , bitangent(Vector3f::ZERO)
   , normal(Vector3f::ZERO)
   , color(Rgba::WHITE)
   , uv0(Vector2f::ZERO)
   , uv1(Vector2f::ONE)
   , boneWeights(Vector4f(1.f, 0.f, 0.f, 0.f))
   , boneIndices(Vector4i(0, 0, 0, 0))
{}

inline bool VertexMaster::operator==(const VertexMaster& rhs)
{
   return ((position == rhs.position)
      && (uv0 == rhs.uv0)
      && (uv1 == rhs.uv1)
      && (normal == rhs.normal)
      && (color == rhs.color)
      && (tangent == rhs.tangent)
      && (bitangent == rhs.bitangent)
      && (boneWeights == rhs.boneWeights)
      && (boneIndices == rhs.boneIndices));
}


//-----------------------------------------------------------------------------------------------
enum PrimitiveType
{
   PRIMITIVE_POINTS,
   PRIMITIVE_LINES,
   PRIMITIVE_LINE_LOOP,
   PRIMITIVE_TRIANGLES,
   PRIMITIVE_TRIANGLE_STRIPS,
   PRIMITIVE_QUADS,
   NUM_PRIMITIVE_TYPES
};


//-----------------------------------------------------------------------------------------------
enum ElementType
{
   INVALID_TYPE = -1,
   INT_ELEMENT,
   FLOAT_ELEMENT,
   UNSIGNED_BYTE_ELEMENT,
   VEC3_ELEMENT,
   VEC4_ELEMENT,
   MAT4_ELEMENT,
   SAMPLER2D_ELEMENT,
   NUM_ELEMENT_TYPES
};


//-----------------------------------------------------------------------------------------------
enum BufferDataUsage
{
   STATIC_DRAW,
   NUM_BUFFER_DATA_USAGES,
};