#include "AudioSystem.h"

AudExec* AudioSystem::instAudExec = NULL;

// Initializes the Audio Executable which includes FMOD core and FMOD studio engine
AudExec::AudExec() {
	mpStudioSystem = NULL;
	AudioSystem::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
	AudioSystem::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

	mpSystem = NULL;
	AudioSystem::ErrorCheck(FMOD::System_Create(&mpSystem));
	AudioSystem::ErrorCheck(mpSystem->init(64, FMOD_INIT_NORMAL, nullptr));

	mnNextChannelId = NULL;
	
}

// Deconstructor to clean up FMOD engine and shutdown
AudExec::~AudExec() {
	AudioSystem::ErrorCheck(mpStudioSystem->unloadAll());
	AudioSystem::ErrorCheck(mpStudioSystem->release());
}

// Update function to check if channels stop playing and clear the channel. Also checks sounds.
void AudExec::Update() {
	vector<ChannelMap::iterator> pStoppedChannels;
	for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it) {
		bool bIsPlaying = false;
		it->second->isPlaying(&bIsPlaying);
		if (!bIsPlaying) {
			pStoppedChannels.push_back(it);
		}
	}
	for (auto& it : pStoppedChannels) {
		mChannels.erase(it);
	}
	AudioSystem::ErrorCheck(mpStudioSystem->update());
}

// Initializes the Audio System using FMOD
AudioSystem::AudioSystem() {
	AudioSystem::InitFMOD();
	LoadSound(src_aud_bgMusic, NULL, true, NULL);
	bgMusicChannelID = PlaySound(src_aud_bgMusic, Vector3(), VolumeTodB(0.5));
}

// Stops playback from all channels from the Audio System and shuts it down
AudioSystem::~AudioSystem() {
	StopAllChannels();
	AudioSystem::Shutdown();
}

// Updates the Audio System depending on FMOD
void AudioSystem::update(EntityManager&, EventManager&, TimeDelta)
{
	UpdateFMOD();
}

// Creates an instance of the Audio Executable to start FMOD
void AudioSystem::InitFMOD() {
	instAudExec = new AudExec;
}

// Updates the instance of the Audio Executable
void AudioSystem::UpdateFMOD() {
	instAudExec->Update();
}

// Loads a sound based on the name and sets parameters about streaming, looping and 3D sound.
void AudioSystem::LoadSound(const string& strSoundName, bool b3d, bool bLooping, bool bStream) {
	auto foundSound = instAudExec->mSounds.find(strSoundName);
	if (foundSound != instAudExec->mSounds.end())
		return;

	FMOD_MODE eMode = FMOD_DEFAULT;
	eMode |= b3d ? FMOD_3D : FMOD_2D;
	eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* pSound = nullptr;
	AudioSystem::ErrorCheck(instAudExec->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
	if (pSound) {
		instAudExec->mSounds[strSoundName] = pSound;
	}
}

// Releases the sound to free up memory
void AudioSystem::UnloadSound(const string& strSoundName) {
	auto foundSound = instAudExec->mSounds.find(strSoundName);
	if (foundSound != instAudExec->mSounds.end())
		return;

	AudioSystem::ErrorCheck(foundSound->second->release());
	instAudExec->mSounds.erase(foundSound);
}

// Checks the sound map for the sound, otherwise loads it. Then creates a channel for the sound and tells it to play.
int AudioSystem::PlaySound(const string& strSoundName, const Vector3& vPosition, float fVolumeDB) {
	int nChannelID = instAudExec->mnNextChannelId++;
	auto foundSound = instAudExec->mSounds.find(strSoundName);
	if (foundSound == instAudExec->mSounds.end()) {
		LoadSound(strSoundName);
		foundSound = instAudExec->mSounds.find(strSoundName);
		if (foundSound == instAudExec->mSounds.end()) {
			return nChannelID;
		}
	}

	FMOD::Channel* pChannel = nullptr;
	AudioSystem::ErrorCheck(instAudExec->mpSystem->playSound(foundSound->second, nullptr, true, &pChannel));
	if (pChannel) {
		FMOD_MODE currMode;
		foundSound->second->getMode(&currMode);
		if (currMode & FMOD_3D) {
			FMOD_VECTOR position = VectorToFMOD(vPosition);
			AudioSystem::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
		}
		AudioSystem::ErrorCheck(pChannel->setVolume(dBToVolume(fVolumeDB)));
		AudioSystem::ErrorCheck(pChannel->setPaused(false));
		instAudExec->mChannels[nChannelID] = pChannel;
	}
	return nChannelID;
}

// Sets the position of where the channel of the sound will come from
void AudioSystem::SetChannelPosition(int nChannelID, const Vector3& vPosition) {
	auto foundChannel = instAudExec->mChannels.find(nChannelID);
	if (foundChannel != instAudExec->mChannels.end())
		return;

	FMOD_VECTOR position = VectorToFMOD(vPosition);
	AudioSystem::ErrorCheck(foundChannel->second->set3DAttributes(&position, NULL));
}

// Sets the volume of the channel of the sound
void AudioSystem::SetChannelVolume(int nChannelID, float fVolumeDB) {
	auto foundChannel = instAudExec->mChannels.find(nChannelID);
	if (foundChannel != instAudExec->mChannels.end())
		return;

	AudioSystem::ErrorCheck(foundChannel->second->setVolume(dBToVolume(fVolumeDB)));
}

// Checks to see if the channel is playing any audio
bool AudioSystem::IsPlaying(int nChannelId) const
{
	bool isPlaying = false;
	instAudExec->mChannels[nChannelId]->isPlaying(&isPlaying);
	return isPlaying;
}

// Loads a bank to store the sounds and information of the event
void AudioSystem::LoadBank(const string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
	auto foundBank = instAudExec->mBanks.find(strBankName);
	if (foundBank != instAudExec->mBanks.end())
		return;

	FMOD::Studio::Bank* pBank;
	AudioSystem::ErrorCheck(instAudExec->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
	if (pBank) {
		instAudExec->mBanks[strBankName] = pBank;
	}
}

// Loads an event to help save memory. Contains the description and instance of the sound.
void AudioSystem::LoadEvent(const std::string& strEventName) {
	auto tFoundit = instAudExec->mEvents.find(strEventName);
	if (tFoundit != instAudExec->mEvents.end())
		return;
	FMOD::Studio::EventDescription* pEventDescription = NULL;
	AudioSystem::ErrorCheck(instAudExec->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
	if (pEventDescription) {
		FMOD::Studio::EventInstance* pEventInstance = NULL;
		AudioSystem::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
		if (pEventInstance) {
			instAudExec->mEvents[strEventName] = pEventInstance;
		}
	}
}

// Plays the event if it has been loaded
void AudioSystem::PlayEvent(const string& strEventName) {
	auto tFoundit = instAudExec->mEvents.find(strEventName);
	if (tFoundit == instAudExec->mEvents.end()) {
		LoadEvent(strEventName);
		tFoundit = instAudExec->mEvents.find(strEventName);
		if (tFoundit == instAudExec->mEvents.end())
			return;
	}
	tFoundit->second->start();
}

// Stops the channel from playing sound
void AudioSystem::StopChannel(int nChannelId)
{
	instAudExec->mChannels[nChannelId]->stop();
}

// Stops the event from playing
void AudioSystem::StopEvent(const string& strEventName, bool bImmediate) {
	auto tFoundIt = instAudExec->mEvents.find(strEventName);
	if (tFoundIt == instAudExec->mEvents.end())
		return;

	FMOD_STUDIO_STOP_MODE eMode;
	eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
	AudioSystem::ErrorCheck(tFoundIt->second->stop(eMode));
}

// Checks to see if an event is playing
bool AudioSystem::IsEventPlaying(const string& strEventName) const {
	auto tFoundIt = instAudExec->mEvents.find(strEventName);
	if (tFoundIt == instAudExec->mEvents.end())
		return false;

	FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
	if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
		return true;
	}
	return false;
}

// Gets the parameter of events 
void AudioSystem::GetEventParameter(const string& strEventName, const string& strParameterName, float* parameter) {
	auto tFoundIt = instAudExec->mEvents.find(strEventName);
	if (tFoundIt == instAudExec->mEvents.end())
		return;

	AudioSystem::ErrorCheck(tFoundIt->second->getParameterByName(strParameterName.c_str(), parameter));
}

// Sets the parameter of events
void AudioSystem::SetEventParameter(const string& strEventName, const string& strParameterName, float fValue) {
	auto tFoundIt = instAudExec->mEvents.find(strEventName);
	if (tFoundIt == instAudExec->mEvents.end())
		return;

	AudioSystem::ErrorCheck(tFoundIt->second->setParameterByName(strParameterName.c_str(), fValue));
}

// Stops all the channels from playing audio
void AudioSystem::StopAllChannels()
{
	for (int i = 0; i < instAudExec->mChannels.size(); i++) {
		StopChannel(i);
	}
}

// Calculates a Vector3 to a FMOD Vector
FMOD_VECTOR AudioSystem::VectorToFMOD(const Vector3& vPosition) {
	FMOD_VECTOR fVec;
	fVec.x = vPosition.x;
	fVec.y = vPosition.y;
	fVec.z = vPosition.z;
	return fVec;
}

// Changes db to linear volume
float  AudioSystem::dBToVolume(float dB)
{
	return powf(10.0f, 0.05f * dB);
}

// changes linear volume to db
float  AudioSystem::VolumeTodB(float volume)
{
	return 20.0f * log10f(volume);
}

// Subscribing to different events that the audio system will catch for triggers
void AudioSystem::configure(EventManager& em)
{
	em.subscribe<AttackInput>(*this);
	em.subscribe<PlayerAttack>(*this);
	em.subscribe<EnemyAttack>(*this);
	em.subscribe<MoveInput>(*this);
	em.subscribe<GameWon>(*this);
	em.subscribe<GameOver>(*this);
	em.subscribe<EnemyAttack>(*this);
	em.subscribe<EntityDestroyedEvent>(*this);
}

//Play Attack Audio when attack input is received
void AudioSystem::receive(const AttackInput& event)
{
	//play sound loads sound in function
	if (gameState == GameState::RUNNING)
		PlaySound(src_aud_playerAttack, Vector3(), VolumeTodB(0.5));
}

//Plays Enemy Hit Audio when enemy hit input is receieved
void AudioSystem::receive(const PlayerAttack& event)
{
	int offset = 1;
	EnemyPos = Vector3();
	switch (event.dir) {
	case EnemyAttack::RIGHT:
		EnemyPos.x = offset;
		break;
	case EnemyAttack::UP:
		EnemyPos.z = offset;
		break;
	case EnemyAttack::LEFT:
		EnemyPos.x = -offset;
		break;
	case EnemyAttack::DOWN:
		EnemyPos.z = -offset;
		break;
	}
	PlaySound(src_aud_enemyHit, EnemyPos, VolumeTodB(0.25));
}

// Plays movement audio when movement input is receieved
void AudioSystem::receive(const MoveInput& event)
{
	if (gameState == GameState::RUNNING) {
		PlaySound(src_aud_playerMoveArmour, Vector3(), VolumeTodB(0.25));
		PlaySound(src_aud_playerMoveStep, Vector3(), VolumeTodB(1));
	}
}

// Plays win audio when game won input is receieved
void AudioSystem::receive(const GameWon& event)
{
	PlaySound(src_aud_win, Vector3(), VolumeTodB(1));
}

// Plays lose audio when game lose input is receieved
void AudioSystem::receive(const GameOver& event)
{
	PlaySound(src_aud_lose, Vector3(), VolumeTodB(1));
}

//Plays Enemy attack and player hit audio when enemy attack input is receieved
void AudioSystem::receive(const EnemyAttack& event)
{
	int offset = 1;
	EnemyPos = Vector3();
	switch (event.dir) {
	case EnemyAttack::RIGHT:
		EnemyPos.x = offset;
		break;
	case EnemyAttack::UP:
		EnemyPos.z = offset;
		break;
	case EnemyAttack::LEFT:
		EnemyPos.x = -offset;
		break;
	case EnemyAttack::DOWN:
		EnemyPos.z = -offset;
		break;
	}
	PlaySound(src_aud_enemyAttack, EnemyPos, VolumeTodB(1));
	PlaySound(src_aud_playerHit, Vector3(), VolumeTodB(0.2));
}

// Plays audio depending on which entity was destroyed, such as picking up treasure or a key
void AudioSystem::receive(const EntityDestroyedEvent& event)
{
	Entity e = event.entity;
	ComponentHandle<GameObject> object = e.component<GameObject>();
	if (object->name == "treasure") {
		PlaySound(src_aud_treasurePickup, Vector3(), VolumeTodB(0.8));
		PlaySound(src_aud_scoreDing, Vector3(), VolumeTodB(0.2));
	}
	else if (object->name == "enemy")
		PlaySound(src_aud_enemyDeath, EnemyPos, VolumeTodB(0.3));
	else if (object->name == "key") {
		PlaySound(src_aud_keyPickup, Vector3(), VolumeTodB(1));
		PlaySound(src_aud_scoreDing, Vector3(), VolumeTodB(0.2));
	}
}

// Error handling for FMOD. Gives an FMOD error number for reference
int AudioSystem::ErrorCheck(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		cout << "FMOD ERROR " << result << endl;
		return 1;
	}
	// cout << "FMOD all good" << endl;
	return 0;
}

// Removes the audio executable instance
void AudioSystem::Shutdown() {
	delete instAudExec;
}