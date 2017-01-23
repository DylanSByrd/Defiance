
//---------------------------------------------------------------------------
// Based on code written by Squirrel Eiserloh
//
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/OpenGLExtensions.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "ThirdParty/stb/stb_image.h"

#include <vector>


//---------------------------------------------------------------------------
STATIC std::map< std::string, Texture* >	Texture::s_textureRegistry;


//---------------------------------------------------------------------------
Texture::Texture(const std::string& imageFilePath)
   : m_openglTextureID(0)
   , m_texelSize(0, 0)
{
   int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
   int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)
   unsigned char* imageData = stbi_load(imageFilePath.c_str(), &m_texelSize.x, &m_texelSize.y, &numComponents, numComponentsRequested);

   if (imageData == nullptr)
   {
      m_openglTextureID = NULL_IMAGE_DATA;
      return;
   }

   // Enable texturing
   glEnable(GL_TEXTURE_2D);

   // Tell OpenGL that our pixel data is single-byte aligned
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   // Ask OpenGL for an unused texName (ID number) to use for this texture
   glGenTextures(1, (GLuint*)&m_openglTextureID);
   
   // Tell OpenGL to bind (set) this as the currently active texture
   glBindTexture(GL_TEXTURE_2D, m_openglTextureID);

   // Set texture clamp vs. wrap (repeat)
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // one of: GL_CLAMP or GL_REPEAT
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // one of: GL_CLAMP or GL_REPEAT
                                                                // Set magnification (texel > pixel) and minification (texel < pixel) filters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // one of: GL_NEAREST, GL_LINEAR
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

   GLenum bufferFormat = GL_RGBA; // the format our source pixel data is currently in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
   if (numComponents == 3)
      bufferFormat = GL_RGB;
   if (numComponents == 1)
      bufferFormat = GL_ALPHA;

   // #FIXME: What happens if numComponents is neither 3 nor 4?

   GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL

   glTexImage2D(			// Upload this pixel data to our new OpenGL texture
      GL_TEXTURE_2D,		// Creating this as a 2d texture
      0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
      internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
      m_texelSize.x,		// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,10], and B is the border thickness [0,1]
      m_texelSize.y,		// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,10], and B is the border thickness [0,1]
      0,					// Border size, in texels (must be 0 or 1)
      bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
      GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color/alpha channel)
      imageData);		// Location of the actual pixel data bytes/buffer

   stbi_image_free(imageData);
}


//-----------------------------------------------------------------------------------------------
Texture::Texture(const unsigned char* texData, Vector2i size)
: m_openglTextureID(0)
, m_texelSize(size)
{
   // Enable texturing
   glEnable(GL_TEXTURE_2D);

   // Tell OpenGL that our pixel data is single-byte aligned
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   // Ask OpenGL for an unused texName (ID number) to use for this texture
   glGenTextures(1, (GLuint*)&m_openglTextureID);

   // Tell OpenGL to bind (set) this as the currently active texture
   glBindTexture(GL_TEXTURE_2D, m_openglTextureID);

   // Set texture clamp vs. wrap (repeat)
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // one of: GL_CLAMP or GL_REPEAT
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // one of: GL_CLAMP or GL_REPEAT

   // Set magnification (texel > pixel) and minification (texel < pixel) filters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // one of: GL_NEAREST, GL_LINEAR
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

   GLenum bufferFormat = GL_RGBA; // the format our source pixel data is currently in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...

   // #FIXME: What happens if numComponents is neither 3 nor 4?

   GLenum internalFormat = bufferFormat; // the format we want the texture to me on the card; allows us to translate into a different texture format as we upload to OpenGL

   glTexImage2D(			// Upload this pixel data to our new OpenGL texture
      GL_TEXTURE_2D,		// Creating this as a 2d texture
      0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
      internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
      size.x,		// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,10], and B is the border thickness [0,1]
      size.y,		// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,10], and B is the border thickness [0,1]
      0,					// Border size, in texels (must be 0 or 1)
      bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
      GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color/alpha channel)
      texData);		// Location of the actual pixel data bytes/buffer
}


//-----------------------------------------------------------------------------------------------
STATIC Texture* Texture::CreateTextureFromData(const unsigned char* texData, Vector2i size, const std::string& name)
{
   Texture* newTexture = new Texture(texData, size);
   if (newTexture->GetTextureID() == NULL_IMAGE_DATA)
   {
      delete newTexture;
      return nullptr;
   }

   s_textureRegistry.insert(std::pair<std::string, Texture*>(name, newTexture));
   return newTexture;
}


//---------------------------------------------------------------------------
// Returns a pointer to the already-loaded texture of a given image file,
//	or nullptr if no such texture/image has been loaded.
//
STATIC Texture* Texture::GetTextureByName(const std::string& imageFilePath)
{
   auto textureIterator = s_textureRegistry.find(imageFilePath);
   
   if (textureIterator == s_textureRegistry.end())
   {
      return nullptr;
   }
   else
   {
      Texture* texture = textureIterator->second;
      return texture;
   }
}


//---------------------------------------------------------------------------
// Finds the named Texture among the registry of those already loaded; if
//	found, returns that Texture*.  If not, attempts to load that texture,
//	and returns a Texture* just created (or nullptr if unable to load file).
//
STATIC Texture* Texture::CreateOrGetTexture(const std::string& imageFilePath)
{
   auto textureIterator = s_textureRegistry.find(imageFilePath);

   if (textureIterator == s_textureRegistry.end())
   {
      Texture* newTexture = new Texture(imageFilePath);
      if (newTexture->GetTextureID() == NULL_IMAGE_DATA)
      {
         delete newTexture;
         return nullptr;
      }

      s_textureRegistry.insert(std::pair<std::string, Texture*>(imageFilePath, newTexture));
      return newTexture;
   }
   else
   {
      Texture* texture = textureIterator->second;
      return texture;
   }
}


//-----------------------------------------------------------------------------------------------//-----------------------------------------------------------------------------------------------
STATIC Texture* Texture::CreateTextureWithFormat(int width, int height, TextureFormat format)
{
   Texture* texToReturn = new Texture();

   glGenTextures(1, (GLuint*)&texToReturn->m_openglTextureID);

   GLenum bufferChannels = GL_RGBA;
   GLenum bufferFormat   = GL_UNSIGNED_INT_8_8_8_8;
   GLenum internalFormat = GL_RGBA;

   if (format == TEXTURE_FORMAT_D24S8)
   {
      bufferChannels = GL_DEPTH_STENCIL;
      bufferFormat   = GL_UNSIGNED_INT_24_8;
      internalFormat = GL_DEPTH24_STENCIL8;
   }
   else if (format != TEXTURE_FORMAT_RGBA8)
   {
      ERROR_AND_DIE("Unsupported texture format.\n");
   }

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texToReturn->m_openglTextureID);
   glTexImage2D(GL_TEXTURE_2D,
      0,
      internalFormat,
      width, height,
      0,
      bufferChannels,
      bufferFormat,
      NULL);

   texToReturn->m_texelSize.x = width;
   texToReturn->m_texelSize.y = height;

   return texToReturn;
}


//-----------------------------------------------------------------------------------------------
STATIC void Texture::FreeAllTextures()
{
   for (auto textureIterator = s_textureRegistry.begin(); textureIterator != s_textureRegistry.end(); ++textureIterator)
   {
      delete textureIterator->second;
   }
}