#pragma once


//-----------------------------------------------------------------------------------------------
enum CullMode
{
   CULL_NONE,
   CULL_FRONT,
   CULL_BACK,
   CULL_ALL,
   NUM_CULL_MODES,
};


//-----------------------------------------------------------------------------------------------
enum BlendFactor
{
   ZERO_BLEND = 0,
   FULL_BLEND,
   SOURCE_COLOR,
   INVERT_SOURCE_COLOR,
   SOURCE_ALPHA,
   INVERT_SOURCE_ALPHA,
   DEST_COLOR,
   INVERT_DEST_COLOR,
   DEST_ALPHA,
   INVERT_DEST_ALPHA,
   NUM_BLEND_FACTORS,
};


//-----------------------------------------------------------------------------------------------
enum FillMode
{
   FILL_POINT,
   FILL_LINE,
   FILL_FULL,
   NUM_FILL_MODES,
};


//-----------------------------------------------------------------------------------------------
enum WindingOrder
{
   WIND_CCW,
   WIND_CW,
   NUM_WINDING_ORDERS,
};


//-----------------------------------------------------------------------------------------------
enum DepthFunc
{
   WRITE_NEVER,
   WRITE_LESS,
   WRITE_LESS_OR_EQUAL,
   WRITE_EQUAL,
   WRITE_GREATER,
   WRITE_NOT_EQUAL,
   WRITE_GREATER_OR_EQUAL,
   WRITE_ALWAYS,
   NUM_DEPTH_FUNCS,
};


//-----------------------------------------------------------------------------------------------
struct RenderState
{
   CullMode     cullMode;
   BlendFactor  sourceBlend;
   BlendFactor  destBlend;
   FillMode     fillMode;
   WindingOrder windingOrder;
   DepthFunc    depthFunc;
   bool         isUsingDepthTest;

   void SetBlendFunc(BlendFactor source, BlendFactor dest);
};


//-----------------------------------------------------------------------------------------------
inline void RenderState::SetBlendFunc(BlendFactor source, BlendFactor dest) 
{ 
   sourceBlend = source; 
   destBlend = dest; 
}