#pragma once

#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector2i.hpp"
#include "Engine/Math/Vector2f.hpp"


//-----------------------------------------------------------------------------------------------
class Texture;


//-----------------------------------------------------------------------------------------------
class SpriteSheet
{
public:
   SpriteSheet(const std::string& imageFilePath, int tilesWide, int tilesHigh);
   ~SpriteSheet();

   Texture* GetTexture() const { return m_spriteSheetTexture; }
   TexBounds GetTexBoundsForSpriteCoords(const SpriteCoords& spriteCoords) const;
   TexBounds GetTexBoundsForSpriteIndex(const SpriteIndex& spriteIndex) const;
   int GetNumSprites() const;

private:
   Texture* m_spriteSheetTexture;
   SpriteCoords m_spriteLayout;
   TexCoords m_texCoordsPerTile;
};