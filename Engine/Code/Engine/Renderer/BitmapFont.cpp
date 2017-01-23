#include <sstream>
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/IO/FileUtils.hpp"


//-----------------------------------------------------------------------------------------------
STATIC std::map< std::string, BitmapFont* >	BitmapFont::s_fontRegistry;


//-----------------------------------------------------------------------------------------------
BitmapFont::BitmapFont()
   : m_fontTexture(nullptr)
   , m_glyphData()
   , m_kerningTable()
{
}


//-----------------------------------------------------------------------------------------------
BitmapFont::~BitmapFont()
{
   for (auto glyphIter : m_glyphData)
   {
      delete glyphIter.second;
   }
}


//-----------------------------------------------------------------------------------------------
bool BitmapFont::Init(const std::string& bitmapFontName)
{
   // Import meta file
   std::string metaFilePath = Stringf("Data/Fonts/%s.fnt", bitmapFontName.c_str());
   std::string metaTextBuffer = "";
   std::vector<unsigned char> test;
   bool isSuccess = LoadBinaryFileToBuffer(metaFilePath, test);
   if (!isSuccess)
   {
      return false;
   }
   
   // TODO: MOVE INTO FILE UTILS
   metaTextBuffer.resize(test.size());
   metaTextBuffer.assign((char*)&test[0], test.size());

   isSuccess = ParseMetaFile(metaTextBuffer);
   return isSuccess;

}


//-----------------------------------------------------------------------------------------------
bool BitmapFont::ParseMetaFile(const std::string& metaTextBuffer)
{
   std::stringstream metaStream(metaTextBuffer);

   bool isSuccess = ReadHeaderData(metaStream);
   if (!isSuccess)
   {
      return false;
   }

   isSuccess = ReadGlyphData(metaStream);
   if (!isSuccess)
   {
      return false;
   }

   isSuccess = ReadKerningData(metaStream);
   if (!isSuccess)
   {
      return false;
   }

   return isSuccess;
}


//-----------------------------------------------------------------------------------------------
bool BitmapFont::ReadHeaderData(std::stringstream& metaStream)
{
   std::string word = "";
   while (word.substr(0, 4).compare("size"))
   {
      metaStream >> word;
   }
   m_fontSize = (float)atof(word.erase(0, 5).c_str());

   while (word.substr(0, 4).compare("file"))
   {
      metaStream >> word;
   }
   std::string textureFile = word.substr(6, word.length() - 7);
   std::string textureFilePath = Stringf("Data/Fonts/%s", textureFile.c_str());
   m_fontTexture = Texture::CreateOrGetTexture(textureFilePath);

   bool isSuccess = (m_fontTexture != nullptr);
   return isSuccess;
}


//-----------------------------------------------------------------------------------------------
bool BitmapFont::ReadGlyphData(std::stringstream& metaStream)
{
   // Find number of glyphs for font
   std::string word = "";
   while (word.substr(0, 5).compare("count"))
   {
      metaStream >> word;
   }

   std::string glyphCountAsString = word.erase(0, 6);
   int glyphCount = atoi(glyphCountAsString.c_str());

   // Read in glyph data
   for (int glyphIndex = 0; glyphIndex < glyphCount; ++glyphIndex)
   {
      // Check for correct file format
      metaStream >> word;
      if (word.compare("char"))
      {
         return false;
      }

      Glyph* currentGlyph = new Glyph();
      metaStream >> word;
      word.erase(0, 3);
      currentGlyph->id = (unsigned char)atoi(word.c_str());

      metaStream >> word;
      word.erase(0, 2);
      currentGlyph->xStart = (short)atoi(word.c_str());

      metaStream >> word;
      word.erase(0, 2);
      currentGlyph->yStart = (short)atoi(word.c_str());

      metaStream >> word;
      word.erase(0, 6);
      currentGlyph->width = (short)atoi(word.c_str());

      metaStream >> word;
      word.erase(0, 7);
      currentGlyph->height = (short)atoi(word.c_str());

      metaStream >> word;
      word.erase(0, 8);
      currentGlyph->xOffset = (char)atoi(word.c_str());

      metaStream >> word;
      word.erase(0, 8);
      currentGlyph->yOffset = (char)atoi(word.c_str());

      metaStream >> word;
      word.erase(0, 9);
      currentGlyph->xAdvance = (char)atoi(word.c_str());

      m_glyphData[currentGlyph->id] = currentGlyph;

      metaStream >> word;
      metaStream >> word;
   }

   return true;
}


//-----------------------------------------------------------------------------------------------
bool BitmapFont::ReadKerningData(std::stringstream& metaStream)
{
   // Kernings are optional; see if file has any
   std::string word = "";
   metaStream >> word;
   if (word.compare("kernings"))
   {
      return true;
   }

   // Kerning count
   metaStream >> word;
   std::string kerningCountAsString = word.erase(0, 6);
   int kerningCount = atoi(word.c_str());

   // Go through kernings
   for (int kerningIndex = 0; kerningIndex < kerningCount; ++kerningIndex)
   {
      // Check for correct file format
      metaStream >> word;
      if (word.compare("kerning"))
      {
         return false;
      }

      KerningPair thisKerningPair;
      metaStream >> word;
      word.erase(0, 6);
      thisKerningPair.leftGlyph = (unsigned char)atoi(word.c_str());

      metaStream >> word;
      word.erase(0, 7);
      thisKerningPair.rightGlyph = (unsigned char)atoi(word.c_str());

      metaStream >> word;
      word.erase(0, 7);
      char kerningAmount = (char)atoi(word.c_str());

      m_kerningTable[thisKerningPair] = kerningAmount;
   }

   return true;
}


//-----------------------------------------------------------------------------------------------
STATIC BitmapFont* BitmapFont::CreateOrGetFont(const std::string& bitmapFontName)
{
   auto fontIterator = s_fontRegistry.find(bitmapFontName);

   if (fontIterator == s_fontRegistry.end())
   {
      BitmapFont* newFont = new BitmapFont();
      bool initSuccessful = newFont->Init(bitmapFontName);
      if (!initSuccessful)
      {
         delete newFont;
         return nullptr;
      }

      s_fontRegistry.insert(std::pair<std::string, BitmapFont*>(bitmapFontName, newFont));
      return newFont;
   }
   else
   {
      BitmapFont* font = fontIterator->second;
      return font;
   }
}


//-----------------------------------------------------------------------------------------------
STATIC void BitmapFont::FreeAllFonts()
{
   for (auto fontIterator = s_fontRegistry.begin(); fontIterator != s_fontRegistry.end(); ++fontIterator)
   {
      delete fontIterator->second;
   }
}


//-----------------------------------------------------------------------------------------------
const Glyph* BitmapFont::GetGlyphForID(unsigned char asciiCode) const
{
   auto glyphIter = m_glyphData.find(asciiCode);
   if (glyphIter == m_glyphData.end())
   {
      return nullptr;
   }

   return glyphIter->second;
}


//-----------------------------------------------------------------------------------------------
char BitmapFont::GetKerningForPair(const Glyph* leftGlyph, const Glyph* rightGlyph) const
{
   if (leftGlyph == nullptr)
   {
      return 0;
   }

   KerningPair pair = { leftGlyph->id, rightGlyph->id };
   auto kerningIter = m_kerningTable.find(pair);
   if (kerningIter == m_kerningTable.end())
   {
      return 0;
   }

   return kerningIter->second;
}