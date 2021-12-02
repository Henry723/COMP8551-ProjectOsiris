#pragma once
#include "../components.hpp"
#include "../Events/EventSystem.h"
#include "AudioSource.h"
#include "../gamestate.h"
#include <FMOD/fmod_studio_common.h>
#include <FMOD/fmod_studio.hpp>
#include <FMOD/fmod.hpp>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

struct Vector3 {
  float x;
  float y;
  float z;
};

struct AudExec {
  AudExec();
  ~AudExec();

  void Update();

  FMOD::Studio::System* mpStudioSystem;
  FMOD::System* mpSystem;

  int mnNextChannelId;

  // Maps for sounds, channels and events for the system.
  typedef map<string, FMOD::Sound*> SoundMap;
  typedef map<int, FMOD::Channel*> ChannelMap;
  typedef map<string, FMOD::Studio::EventInstance*> EventMap;
  typedef map<string, FMOD::Studio::Bank*> BankMap;

  BankMap mBanks;
  EventMap mEvents;
  SoundMap mSounds;
  ChannelMap mChannels;
};

class AudioSystem : public System<AudioSystem> , public Receiver<AudioSystem>{
public:
	static void InitFMOD();
	static void UpdateFMOD();
	static void Shutdown();
	static int ErrorCheck(FMOD_RESULT result);

	void update(EntityManager&, EventManager&, TimeDelta) override;
	void configure(EventManager& em) override;
	void receive(const AttackInput& event);

	AudioSystem();
	~AudioSystem();

	void LoadBank(const string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	void LoadEvent(const string& strEventName);
	void LoadSound(const string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
	void UnloadSound(const string& strSoundName);
	void Set3dListenerOrientation(const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumeDB = 0.0f);
	int PlaySound(const string& strSoundName, const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumeDB = 0.0f);
	void PlayEvent(const string& strEventName);
	void StopChannel(int nChannelId);
	void StopEvent(const string& strEventName, bool bImmediate = false);
	void GetEventParameter(const string& strEventName, const string& strEventParameter, float* parameter);
	void SetEventParameter(const string& strEventName, const string& strParameterName, float fvalue);
	void StopAllChannels();
	void SetChannelPosition(int nChannelID, const Vector3& vPosition);
	void SetChannelVolume(int nChannelId, float fvolumeDB);
	bool IsPlaying(int nChannelId) const;
	bool IsEventPlaying(const string& strEventName) const;
	float dBToVolume(float db);
	float VolumeTodB(float volume);
	FMOD_VECTOR VectorToFMOD(const Vector3& vPosition);
private:
	static AudExec* instAudExec;
	int bgMusicChannelID;
};