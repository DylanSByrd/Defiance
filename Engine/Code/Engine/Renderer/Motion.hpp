#pragma once

#include <string>
#include "Engine/Math/Matrix4x4.hpp"


//-----------------------------------------------------------------------------------------------
class Skeleton;
class FileBinaryWriter;
class FileBinaryReader;


//-----------------------------------------------------------------------------------------------
class Motion
{
public:
   enum ExtrapolationWrapMode
   {
      CLAMP,
      LOOP,
      PING_PONG,
      NUM_EXTRAPOLATION_WRAP_MODES,
   };


   Motion();
   Motion(const std::string motionName, ExtrapolationWrapMode wrapMode, float totalLengthSeconds, float frameRate, Skeleton* skeleton);
   ~Motion();

   void GetFrameIndicesWithBlend(uint32_t* outFrameIndex0, uint32_t* outFrameIndex1, float* outBlend, float inTime);
   Matrix4x4* GetJointKeyFrames(uint32_t jointIndex);
   float ApplyExtrapolationToTime(float time) const;
   void ApplyMotionToSkeleton(Skeleton* skeleton, float time);

   uint32_t GetFrameCount() const { return m_frameCount; }
   void SetWrapMode(ExtrapolationWrapMode mode) { m_extrapolationWrapMode = mode; }

   // FileIO
   static const uint8_t FILE_VERSION;

   void WriteToFile(const std::string& fileName);
   bool ReadFromFile(const std::string& fileName);


private:
   std::string m_name;
   ExtrapolationWrapMode m_extrapolationWrapMode;
   float m_totalLengthSeconds;
   float m_frameRate;
   float m_frameTime;

   Matrix4x4* m_keyFrames;
   int m_jointCount;
   int m_frameCount;

   // FileIO
   void WriteToStream(FileBinaryWriter* writer);
   void WriteKeyFrames(FileBinaryWriter* writer);
   void ReadFromStream(FileBinaryReader* reader);
   void ReadKeyFrames(FileBinaryReader* reader);
};