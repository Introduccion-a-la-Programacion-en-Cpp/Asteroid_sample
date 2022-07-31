#pragma once
#include "StructsDef.h"

#define USE_MUSIC

enum MUSIC_SFX{
	BGM_MAIN_MENU = 0,
	BGM_GAMEPLAY,
	BGM_CREDITS,
	BGM_TOTAL
};

void loadAllMusicAssets(ResourceManager& resource);
void unloadAllMusicAssets(ResourceManager& resource);
void PlayBGM(int bgmID, ResourceManager& resource);

void StopBGM();