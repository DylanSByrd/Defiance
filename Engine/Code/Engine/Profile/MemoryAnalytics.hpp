#pragma once


//-----------------------------------------------------------------------------------------------
extern size_t g_numAllocations;
extern size_t g_totalAllocated;


//-----------------------------------------------------------------------------------------------
class MemoryAnalytics
{
public:
   static void MemoryAnalyticsStartup();
   static void MemoryAnalyticsShutdown();
};