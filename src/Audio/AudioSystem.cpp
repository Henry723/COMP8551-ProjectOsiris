#include "AudioSystem.h"

AudExec::AudExec() {
	mpStudioSystem = NULL;
	AudioSystem::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
	AudioSystem::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

	mpSystem = NULL;
//	AudioSystem::ErrorCheck(mpStudioSystem->getLowLevelSystem(&mpSystem));
//	This isn't in any of the API references? Not in FMOD::Studio::System.

}

AudExec::~AudExec() {
	AudioSystem::ErrorCheck(mpStudioSystem->unloadAll());
	AudioSystem::ErrorCheck(mpStudioSystem->release());
}

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

AudExec* instAudExec = nullptr;

void AudioSystem::Init() {
	instAudExec = new AudExec;
}

void AudioSystem::Update() {
	instAudExec->Update();
}

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

void AudioSystem::UnloadSound(const string& strSoundName) {
	auto foundSound = instAudExec->mSounds.find(strSoundName);
	if (foundSound != instAudExec->mSounds.end())
		return;

	AudioSystem::ErrorCheck(foundSound->second->release());
	instAudExec->mSounds.erase(foundSound);
}

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
		AudioSystem::ErrorCheck(pChannel->setVolume(DBToVolume(fVolumeDB)));
		AudioSystem::ErrorCheck(pChannel->setPaused(false));
		instAudExec->mChannels[nChannelID] = pChannel;
	}
	return nChannelID;
}

void AudioSystem::SetChannelPosition(int nChannelID, const Vector3& vPosition) {
	auto foundChannel = instAudExec->mChannels.find(nChannelID);
	if (foundChannel != instAudExec->mChannels.end())
		return;

	FMOD_VECTOR position = VectorToFMOD(vPosition);
	AudioSystem::ErrorCheck(foundChannel->second->set3DAttributes(&position, NULL));
}

void AudioSystem::SetChannelVolume(int nChannelID, float fVolumeDB) {
	auto foundChannel = instAudExec->mChannels.find(nChannelID);
	if (foundChannel != instAudExec->mChannels.end())
		return;

	AudioSystem::ErrorCheck(foundChannel->second->setVolume(DBToVolume(fVolumeDB)));
}

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