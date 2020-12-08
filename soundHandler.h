#pragma once
#include <stdlib.h>
#include <string>
#include "yssimplesound.h"

using namespace std;

class Sound {
private:
	YsSoundPlayer player;
	YsSoundPlayer::SoundData sound;
	string soundFile;
public:
	Sound(string file);
	void playOnce();
	void playLoop();
	void stop();
	void setVolume(float vol);
	bool isPlaying();
};