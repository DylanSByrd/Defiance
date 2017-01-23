#pragma once

#include <string>
#include <map>
#include "Engine/Renderer/Glyph.hpp"
#include "Engine/Renderer/KerningPair.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
// #TODO - Make a font material!

class BitmapFont
{
public:
   static BitmapFont* CreateOrGetFont(const std::string& bitmapFontName);
   static void FreeAllFonts();

   const Texture* GetTexture() const { return m_fontTexture; }
   const Glyph* GetGlyphForID(unsigned char asciiCode) const;
   Vector2i GetTextureDimensions() const { return m_fontTexture->GetTexelSize(); }
   char GetKerningForPair(const Glyph* leftGlyph, const Glyph* rightGlyph) const;
   float GetFontSize() const { return m_fontSize; }

private:
   BitmapFont();
   ~BitmapFont();
   
   bool Init(const std::string& bitmapFontName);
   bool ParseMetaFile(const std::string& metaTextBuffer);
   bool ReadHeaderData(std::stringstream& metaStream);
   bool ReadGlyphData(std::stringstream& metaStream);
   bool ReadKerningData(std::stringstream& metaStream);

   static std::map<std::string, BitmapFont*> s_fontRegistry;
   //SpriteSheet* m_spriteSheet;

   float m_fontSize;
   Texture* m_fontTexture;
   std::map<unsigned char, Glyph*> m_glyphData;
   std::map<KerningPair, char> m_kerningTable;
};