#pragma once


//-----------------------------------------------------------------------------------------------
struct KerningPair
{
   unsigned char leftGlyph;
   unsigned char rightGlyph;
};

bool operator<(const KerningPair& lhs, const KerningPair& rhs);