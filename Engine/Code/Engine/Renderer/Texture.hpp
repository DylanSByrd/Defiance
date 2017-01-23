#pragma once

#include <string>
#include <map>
#include "Engine/Math/Vector2i.hpp"


//-----------------------------------------------------------------------------------------------
const int NULL_IMAGE_DATA = -1;


//-----------------------------------------------------------------------------------------------
enum TextureFormat
{
   TEXTURE_FORMAT_RGBA8,
   TEXTURE_FORMAT_D24S8,
   NUM_TEXTURE_FORMATS
};

//-----------------------------------------------------------------------------------------------
class Texture
{
public:
   static Texture* GetTextureByName(const std::string& imageFilePath);
   static Texture* CreateTextureFromData(const unsigned char* texData, Vector2i size, const std::string& name);
   static Texture* CreateOrGetTexture(const std::string& imageFilePath);
   static Texture* CreateTextureWithFormat(int width, int height, TextureFormat format);

   static void FreeAllTextures();

   int GetTextureID() const { return m_openglTextureID; }
   Vector2i GetTexelSize() const { return m_texelSize; }

private:
   Texture(const std::string& imageFilePath);
   Texture(const unsigned char* texData, Vector2i size);
   Texture() {};
   static std::map< std::string, Texture*> s_textureRegistry;
   int m_openglTextureID;
   Vector2i m_texelSize;
};