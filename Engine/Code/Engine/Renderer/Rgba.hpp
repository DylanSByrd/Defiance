#pragma once


//-----------------------------------------------------------------------------------------------
class Vector4f;


//-----------------------------------------------------------------------------------------------
class Rgba
{
public:
   Rgba() {}
   Rgba(unsigned char initR, unsigned char initG, unsigned char initB, unsigned char initA) : r(initR), g(initG), b(initB), a(initA) {}
   Rgba(unsigned char uniformColor, unsigned char initA) : r(uniformColor), g(uniformColor), b(uniformColor), a(initA) {}
   Rgba(unsigned char uniformColor) : r(uniformColor), g(uniformColor), b(uniformColor), a(255) {}
   ~Rgba() {}

   bool operator==(const Rgba& rhs) const;
   Rgba operator*(float scale) const;
   Rgba& operator*=(float scale);

   Vector4f GetNormalized() const;

   static const Rgba BLACK;
   static const Rgba LIGHT_GREY;
   static const Rgba WHITE;
   static const Rgba RED;
   static const Rgba GREEN;
   static const Rgba BLUE;
   static const Rgba YELLOW;
   static const Rgba CYAN;
   static const Rgba ORANGE;
   static const Rgba VERMILLION;
   static const Rgba MAGENTA;

   unsigned char r;
   unsigned char g;
   unsigned char b;
   unsigned char a;
};