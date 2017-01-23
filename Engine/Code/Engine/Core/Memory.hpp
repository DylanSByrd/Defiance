#pragma once

//-----------------------------------------------------------------------------------------------
void* operator new(size_t numBytes);
void* operator new[](size_t numBytes);
void operator delete(void* ptr);
void operator delete[](void* ptr);


//-----------------------------------------------------------------------------------------------
