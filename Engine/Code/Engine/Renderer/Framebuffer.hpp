#pragma once

#include <vector>
#include "Engine/Renderer/RendererCommon.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
class Framebuffer
{
public:
   Framebuffer(GLuint handle, int width, int height, const std::vector<Texture*>& colorTargets, Texture* const depthStencilTarget);
   ~Framebuffer();

   GLuint GetHandle() const { return m_handle; }
   int GetPixelWidth() const { return m_pixelWidth; }
   int GetPixelHeight() const { return m_pixelHeight; }
   const std::vector<Texture*>& GetColorTargets() const { return m_colorTargets; } // #TODO - try removing me
   Texture* GetColorTargetAtIndex(int index) const { return m_colorTargets[index]; }
   int GetNumColorTargets() const { return m_colorTargets.size(); }
   Texture* GetDepthStencilTarget() const { return m_depthStencilTarget; }

   void SetHandle(GLuint handle) { m_handle = handle; }
   void SetPixelWidth(int width) { m_pixelWidth = width; }
   void SetPixelHeight(int height) { m_pixelHeight = height; }
   void SetColorTargets(const std::vector<Texture*>& targets) { m_colorTargets = targets; }
   void SetDepthStencilTarget(Texture* const target) { m_depthStencilTarget = target; }


private:
   GLuint m_handle;

   std::vector<Texture*> m_colorTargets;
   Texture* m_depthStencilTarget;

   int m_pixelWidth;
   int m_pixelHeight;
};