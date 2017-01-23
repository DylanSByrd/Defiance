#include "Engine/Core/Tokenizer.hpp"


//-----------------------------------------------------------------------------------------------
Tokenizer::Tokenizer(const std::string& string, const std::string& delimiters)
   : m_originalString(string)
   , m_delimiters(delimiters)
   , m_currentOffset(0)
   , m_nextOffset(-1)
   , m_isFinished(false)
{}


//-----------------------------------------------------------------------------------------------
std::string Tokenizer::GetNextToken()
{
   if (m_isFinished)
   {
      return "";
   }

   m_currentOffset = m_nextOffset + 1;
   m_nextOffset = m_originalString.find_first_of(m_delimiters, m_currentOffset);

   if (m_nextOffset == (int)std::string::npos)
   {
      m_isFinished = true;
   }

   return m_originalString.substr(m_currentOffset, m_nextOffset - m_currentOffset);
}