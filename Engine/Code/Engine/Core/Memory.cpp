#include "Engine/Core/Memory.hpp"
#include "Engine/Profile/MemoryAnalytics.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
void* operator new(size_t numBytes)
{
   size_t* ptr = (size_t*)malloc(sizeof(size_t) + numBytes);
   /*DebuggerPrintf("Alloc %p of %u bytes.\n", ptr, numBytes);*/
   ++g_numAllocations;
   g_totalAllocated += numBytes;

   *ptr = numBytes;
   ++ptr;
   return ptr;
}


//-----------------------------------------------------------------------------------------------
void* operator new[](size_t numBytes)
{
   size_t* ptr = (size_t*)malloc(sizeof(size_t) + numBytes);
   /*DebuggerPrintf("Alloc %p of %u bytes.\n", ptr, numBytes);*/
   ++g_numAllocations;
   g_totalAllocated += numBytes;

   *ptr = numBytes;
   ++ptr;
   return ptr;
}


//-----------------------------------------------------------------------------------------------
void operator delete(void* ptr)
{
   size_t* ptrSize = (size_t*)ptr;
   --ptrSize;
   size_t numBytes = *ptrSize;
   g_totalAllocated -= numBytes;
   --g_numAllocations;

   free(ptrSize);
}


//-----------------------------------------------------------------------------------------------
void operator delete[](void* ptr)
{
   size_t* ptrSize = (size_t*)ptr;
   --ptrSize;
   size_t numBytes = *ptrSize;
   g_totalAllocated -= numBytes;
   --g_numAllocations;

   free(ptrSize);
}