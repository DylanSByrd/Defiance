#include "Engine/Renderer/Framebuffer.hpp"


//-----------------------------------------------------------------------------------------------
Framebuffer::Framebuffer(GLuint handle, int width, int height, const std::vector<Texture*>& colorTargets, Texture* const depthStencilTarget)
   : m_handle(handle)
   , m_pixelWidth(width)
   , m_pixelHeight(height)
   , m_colorTargets(colorTargets)
   , m_depthStencilTarget(depthStencilTarget)
{

}


//-----------------------------------------------------------------------------------------------
Framebuffer::~Framebuffer()
{
   for (Texture* colorTarget : m_colorTargets)
   {
      delete colorTarget;
   }

   if (m_depthStencilTarget != nullptr)
   {
      delete m_depthStencilTarget;
   }
}