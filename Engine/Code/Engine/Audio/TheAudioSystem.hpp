#ifndef INCLUDED_AUDIO
#define INCLUDED_AUDIO
#pragma once

//---------------------------------------------------------------------------
#pragma comment( lib, "ThirdParty/fmod/fmodex_vc" ) // Link in the fmodex_vc.lib static library

//---------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>


//---------------------------------------------------------------------------
typedef unsigned int SoundID;
typedef void* AudioChannelHandle;
const unsigned int MISSING_SOUND_ID = 0xffffffff;


//-----------------------------------------------------------------------------------------------
class TheAudioSystem;
extern TheAudioSystem* g_theAudioSystem;


//-----------------------------------------------------------------------------------------------
class TheAudioSystem
{
public:
   TheAudioSystem();
   virtual ~TheAudioSystem();
   SoundID CreateOrGetSound( const std::string& soundFileName );
   void StopChannel(AudioChannelHandle channel);
   AudioChannelHandle PlaySound(SoundID soundID, float volumeLevel = 1.f);
   AudioChannelHandle PlayLoopingSound(SoundID, float volumeLevel = 1.f);
   void Update(); // Must be called at regular intervals (e.g. every frame)

protected:
   void InitializeFMOD();
   void ValidateResult( FMOD_RESULT result );

protected:
   FMOD::System*						m_fmodSystem;
   std::map< std::string, SoundID >	m_registeredSoundIDs;
   std::vector< FMOD::Sound* >			m_registeredSounds;
};


//---------------------------------------------------------------------------
void InitializeAudio();


#endif // INCLUDED_AUDIO
