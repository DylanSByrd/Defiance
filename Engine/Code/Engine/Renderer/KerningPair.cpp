#include "Engine/Renderer/KerningPair.hpp"


//-----------------------------------------------------------------------------------------------
bool operator<(const KerningPair& lhs, const KerningPair& rhs)
{
   if (lhs.leftGlyph < rhs.leftGlyph)
   {
      return true;
   }

   if (rhs.leftGlyph < lhs.leftGlyph)
   {
      return false;
   }

   return (lhs.rightGlyph < rhs.rightGlyph);
}