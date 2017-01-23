#pragma once

#include <string>


//-----------------------------------------------------------------------------------------------
class Tokenizer
{
public:
   Tokenizer(const std::string& string, const std::string& delimiters);

   std::string GetNextToken();
   bool IsFinished() const { return m_isFinished; }

private:
   std::string m_originalString;
   std::string m_delimiters;
   int m_currentOffset;
   int m_nextOffset;
   bool m_isFinished;
};