#pragma once
#include "../components.hpp"
#include <FMOD/fmod_studio_common.h>
#include <FMOD/fmod_studio.hpp>
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

struct Implementation {
  Implementation();
  ~Implementation();

  void Update();

  FMOD::Studio::System* mpStudioSystem;
  FMOD::System* mpSystem;

  int mnNextChannelId;

  typedef map<string, FMOD::Sound*> SoundMap;
  typedef map<int, FMOD::Channel*> ChannelMap;
  typedef map<string, FMOD::Studio::EventInstance*> EventMap;
  typedef map<string, FMOD::Studio::Bank*> BankMap;

  BankMap mBanks;
  EventMap mEvents;
  SoundMap mSounds;
  ChannelMap mChannels;
};

class AudioSystem : public System<AudioSystem> {
public:

private:

};