#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector4f.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const Rgba Rgba::BLACK(0, 0, 0, 255);
STATIC const Rgba Rgba::LIGHT_GREY(179, 179, 179, 255);
STATIC const Rgba Rgba::WHITE(255, 255, 255, 255);
STATIC const Rgba Rgba::RED(255, 0, 0, 255);
STATIC const Rgba Rgba::GREEN(0, 255, 0, 255);
STATIC const Rgba Rgba::BLUE(0, 0, 255, 255);
STATIC const Rgba Rgba::YELLOW(255, 255, 0, 255);
STATIC const Rgba Rgba::CYAN(0, 255, 255, 255);
STATIC const Rgba Rgba::ORANGE(255, 128, 0, 255);
STATIC const Rgba Rgba::VERMILLION(227, 97, 59, 255);
STATIC const Rgba Rgba::MAGENTA(255, 0, 255, 255);


//-----------------------------------------------------------------------------------------------
bool Rgba::operator==(const Rgba& rhs) const
{
   if (r == rhs.r
      && g == rhs.g
      && b == rhs.b
      && a == rhs.a)
   {
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------------------------
Rgba Rgba::operator*(float scale) const
{
   Rgba newColor;
   newColor.r = (unsigned char)(r * scale);
   newColor.g = (unsigned char)(g * scale);
   newColor.b = (unsigned char)(b * scale);
   return newColor;
}


//-----------------------------------------------------------------------------------------------
Rgba& Rgba::operator*=(float scale)
{
   r = (unsigned char)(r * scale);
   g = (unsigned char)(g * scale);
   b = (unsigned char)(b * scale);
   return *this;
}


//-----------------------------------------------------------------------------------------------
Vector4f Rgba::GetNormalized() const
{
   float newR = (float)r / 255.f;
   float newG = (float)g / 255.f;
   float newB = (float)b / 255.f;
   float newA = (float)a / 255.f;

   Vector4f normalizedColor(newR, newG, newB, newA);
   return normalizedColor;
}