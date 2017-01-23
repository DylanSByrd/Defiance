#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Motion.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/IO/FileBinaryWriter.hpp"
#include "Engine/IO/FileBinaryReader.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Engine/Debug/Console.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
STATIC const uint8_t Motion::FILE_VERSION = 1;


//-----------------------------------------------------------------------------------------------
Motion::Motion()
   : m_name("Uninitialized Motion")
   , m_extrapolationWrapMode(CLAMP)
   , m_frameCount(0)
   , m_jointCount(0)
   , m_totalLengthSeconds(0.f)
   , m_frameTime(0.f)
   , m_frameRate(0)
   , m_keyFrames(nullptr)
{}


//-----------------------------------------------------------------------------------------------
Motion::Motion(const std::string motionName, ExtrapolationWrapMode wrapMode, float totalLengthSeconds, float frameRate, Skeleton* skeleton)
   : m_name(motionName)
   , m_extrapolationWrapMode(wrapMode)
   , m_frameCount((int)ceil(frameRate * totalLengthSeconds) + 1)
   , m_jointCount(skeleton->GetJointCount())
   , m_totalLengthSeconds(totalLengthSeconds)
   , m_frameTime(1.f / frameRate)
   , m_frameRate(frameRate)
   , m_keyFrames(nullptr)
{
   m_keyFrames = new Matrix4x4[m_frameCount * m_jointCount];
}


//-----------------------------------------------------------------------------------------------
Motion::~Motion()
{
   delete[] m_keyFrames;
}


//-----------------------------------------------------------------------------------------------
void Motion::GetFrameIndicesWithBlend(uint32_t* outFrameIndex0, uint32_t* outFrameIndex1, float* outBlend, float inTime)
{
   *outFrameIndex0 = (uint32_t)floor(inTime / m_frameTime);
   *outFrameIndex1 = *outFrameIndex0 + 1;

   if (*outFrameIndex0 >= (uint32_t)(m_frameCount - 1))
   {
      *outFrameIndex0 = m_frameCount - 1;
      *outFrameIndex1 = m_frameCount - 1;
      *outBlend = 0.f;
   }
   else if (*outFrameIndex0 == (uint32_t)(m_frameCount - 2))
   {
      float lastFrameTime = m_totalLengthSeconds - (m_frameTime * *outFrameIndex0);
      *outBlend = fmodf(inTime, m_frameTime) / lastFrameTime;
      *outBlend = ClampZeroToOnef(*outBlend);
   }
   else
   {
      *outBlend = fmodf(inTime, m_frameTime) / m_frameTime;
   }
}


//-----------------------------------------------------------------------------------------------
Matrix4x4* Motion::GetJointKeyFrames(uint32_t jointIndex)
{
   return &m_keyFrames[m_frameCount * jointIndex];
}


//-----------------------------------------------------------------------------------------------
float Motion::ApplyExtrapolationToTime(float time) const
{
   switch (m_extrapolationWrapMode)
   {
   case CLAMP:
   {
      if (time > m_totalLengthSeconds)
      {
         time = m_totalLengthSeconds;
      }
      return time;
   }

   case LOOP:
   {
      time = fmodf(time, m_totalLengthSeconds);
      return time;
   }

   case PING_PONG:
   {
      time = fmodf(time, m_totalLengthSeconds * 2.f);
      if (time > m_totalLengthSeconds)
      {
         float temp = fmodf(time, m_totalLengthSeconds);
         time = m_totalLengthSeconds - temp;
      }
      return time;
   }
   }

   return time;
}


//-----------------------------------------------------------------------------------------------
void Motion::ApplyMotionToSkeleton(Skeleton* skeleton, float time)
{
   uint32_t frame0;
   uint32_t frame1;
   float blend;

   time = ApplyExtrapolationToTime(time);

   GetFrameIndicesWithBlend(&frame0, &frame1, &blend, time);

   uint32_t jointCount = skeleton->GetJointCount();
   for (uint32_t jointIndex = 0; jointIndex < jointCount; ++jointIndex)
   {
      Matrix4x4* jointKeyFrames = GetJointKeyFrames(jointIndex);
      Matrix4x4& mat0 = jointKeyFrames[frame0];
      Matrix4x4& mat1 = jointKeyFrames[frame1];

      Matrix4x4 newMat = Matrix4x4::MatLerp(mat0, mat1, blend);

      skeleton->SetJointWorldTransform(jointIndex, newMat);
   }
}


//-----------------------------------------------------------------------------------------------
void Motion::WriteToFile(const std::string& fileName)
{
   FileBinaryWriter writer;
   writer.OpenFile(fileName);

   WriteToStream(&writer);
   
   writer.CloseFile();
}


//-----------------------------------------------------------------------------------------------
bool Motion::ReadFromFile(const std::string& fileName)
{
   bool wasReadSuccessful = true;

   FileBinaryReader reader;
   if (!reader.OpenFile(fileName))
   {
      g_theConsole->ConsolePrintf(Stringf("Error: Could not find file %s", fileName.c_str()), Rgba::RED);
      return !wasReadSuccessful;
   }

   ReadFromStream(&reader);

   reader.CloseFile();
   return wasReadSuccessful;
}


//-----------------------------------------------------------------------------------------------
void Motion::WriteToStream(FileBinaryWriter* writer)
{
   writer->Write(FILE_VERSION);

   writer->WriteString(m_name);
   writer->Write(m_totalLengthSeconds);

   // This tells us how many keyframes
   writer->Write(m_frameRate);
   writer->Write(m_jointCount);

   WriteKeyFrames(writer);
}


//-----------------------------------------------------------------------------------------------
void Motion::WriteKeyFrames(FileBinaryWriter* writer)
{
   // RLE Encoding
   size_t totalNumKeyFrames = m_jointCount * m_frameCount;
   uint8_t currentRunLength = 0;
   Matrix4x4 currentKeyFrame = *m_keyFrames;

   for (size_t frameIndex = 0; frameIndex < totalNumKeyFrames; ++frameIndex)
   {
      if (m_keyFrames[frameIndex] != currentKeyFrame)
      {
         if (currentRunLength != 0)
         {
            writer->Write(currentRunLength);
            writer->Write(currentKeyFrame);
         }

         currentKeyFrame = m_keyFrames[frameIndex];
         currentRunLength = 1;
      }
      else if (currentRunLength == 255)
      {
         writer->Write(currentRunLength);
         writer->Write(currentKeyFrame);
         currentRunLength = 1;
      }
      else
      {
         currentRunLength++;
      }
   }

   if (currentRunLength > 0)
   {
      writer->Write(currentRunLength);
      writer->Write(currentKeyFrame);
   }
}


void Motion::ReadFromStream(FileBinaryReader* reader)
{
   uint8_t incomingFileVersion;
   reader->Read(&incomingFileVersion);

   // #TODO - eventually be more backwards compatible
   ASSERT_OR_DIE(incomingFileVersion == FILE_VERSION, "Error: incoming file does not match the current file version!");

   reader->ReadString(&m_name);
   reader->Read(&m_totalLengthSeconds);

   // This tells us how many keyframes
   reader->Read(&m_frameRate);
   reader->Read(&m_jointCount);

   // Init values based on info read in
   m_frameCount = (int)ceil(m_frameRate * m_totalLengthSeconds) + 1;
   m_frameTime = 1.f / m_frameRate;

   ReadKeyFrames(reader);
}


void Motion::ReadKeyFrames(FileBinaryReader* reader)
{
   size_t numKeyFrames = m_jointCount * m_frameCount;

   // ASSUME m_keyFrames IS NULL
   m_keyFrames = new Matrix4x4[numKeyFrames];

   size_t frameIndex = 0;
   uint8_t currentRunLength;
   Matrix4x4 currentKeyFrame;
   
   while (frameIndex < numKeyFrames)
   {
      reader->Read(&currentRunLength);
      reader->Read(&currentKeyFrame);
      
      while (currentRunLength != 0)
      {
         m_keyFrames[frameIndex] = currentKeyFrame;
         --currentRunLength;
         ++frameIndex;
      }
   }
}