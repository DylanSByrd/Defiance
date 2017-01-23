#pragma once

#include <string>
#include "Engine/Renderer/Rgba.hpp"


//-----------------------------------------------------------------------------------------------
struct ConsoleString
{
   std::string content;
   Rgba color;

   ConsoleString(const std::string& str, const Rgba& c) : content(str), color(c) {}
};