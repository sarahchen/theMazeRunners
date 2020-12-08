/*
Created by: Nate Winneg
Interacts with yssimplesound
to  make things easier.
*/

#include "soundHandler.h"

Sound::Sound(string file)
{
	soundFile = file;
}

void Sound::playOnce()
{
	// play the sound one time;
	if (YSOK == sound.LoadWav(soundFile.c_str())) {
		player.Start();
		player.PlayOneShot(sound);
	}
}

void Sound::playLoop()
{
	if (YSOK == sound.LoadWav(soundFile.c_str())) {
		player.Start();
		player.PlayBackground(sound);
	}
}

void Sound::stop()
{
	player.Stop(sound);
}

void Sound::setVolume(float vol)
{
	player.SetVolume(sound, vol);
}

bool Sound::isPlaying()
{
	if (player.IsPlaying(sound) == YSTRUE)
		return true;
	else
		return false;
}
