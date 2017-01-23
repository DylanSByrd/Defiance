#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/Vector2f.hpp"
#include "Engine/Math/AABB2.hpp"


//-----------------------------------------------------------------------------------------------
SpriteSheet::SpriteSheet(const std::string& imageFilePath, int tilesWide, int tilesHigh)
   : m_spriteSheetTexture(Texture::CreateOrGetTexture(imageFilePath))
   , m_spriteLayout(SpriteCoords(tilesWide, tilesHigh))
   , m_texCoordsPerTile(TexCoords(1.f / (float)tilesWide, 1.f / (float)tilesHigh))
{
}


//-----------------------------------------------------------------------------------------------
SpriteSheet::~SpriteSheet()
{

}


//-----------------------------------------------------------------------------------------------
TexBounds SpriteSheet::GetTexBoundsForSpriteCoords(const SpriteCoords& spriteCoords) const
{
   TexBounds texBounds;
   texBounds.mins.x = m_texCoordsPerTile.x * (float)spriteCoords.x;
   texBounds.mins.y = m_texCoordsPerTile.y * (float)spriteCoords.y;
   texBounds.maxs.x = texBounds.mins.x + m_texCoordsPerTile.x;
   texBounds.maxs.y = texBounds.mins.y + m_texCoordsPerTile.y;
   return texBounds;
}


//-----------------------------------------------------------------------------------------------
TexBounds SpriteSheet::GetTexBoundsForSpriteIndex(const SpriteIndex& spriteIndex) const
{
   SpriteCoords indexAsCoords(spriteIndex % m_spriteLayout.x, spriteIndex / m_spriteLayout.x);
   return GetTexBoundsForSpriteCoords(indexAsCoords);
}


//-----------------------------------------------------------------------------------------------
int SpriteSheet::GetNumSprites() const
{
   return (m_spriteLayout.x * m_spriteLayout.y);
}