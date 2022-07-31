#include "MusicDef.h"


void loadAllMusicAssets(ResourceManager& resource) {

	BgmAssets& musicAssets = *(resource.musicAssets);
	musicAssets.resize(BGM_TOTAL); // Para garantizar que se cargan todas las musicas

	// Cargo BGM de los creditos
	string soundFilePath = "assets/bgm/littleidea.mp3";
	Mix_Music* music;
	music = Mix_LoadMUS(soundFilePath.c_str());

	// Voy a definir y reutilizar esta variable
	Bgm temp_bgm;
	temp_bgm.music = music;

	musicAssets.insert(musicAssets.begin() + BGM_CREDITS, (temp_bgm));

	// BGM de Main Menu
	soundFilePath = "assets/bgm/epic.mp3";
	music = Mix_LoadMUS(soundFilePath.c_str());
	temp_bgm.music = music;
	musicAssets.insert(musicAssets.begin() + BGM_MAIN_MENU, (temp_bgm));

	// BGM de Gameplay
	soundFilePath = "assets/bgm/extremeaction.mp3";
	music = Mix_LoadMUS(soundFilePath.c_str());
	temp_bgm.music = music;
	musicAssets.insert(musicAssets.begin() + BGM_GAMEPLAY, (temp_bgm));
}

void unloadAllMusicAssets(ResourceManager& resource) {
	//TODO: complete implementation
}

void PlayBGM(int bgmID, ResourceManager& resource) {

	BgmAssets& musicAssets = *(resource.musicAssets);

	Mix_PlayMusic(musicAssets[bgmID].music, -1);
}

void StopBGM() {
	// Por ahora solo la detenemos...
	Mix_HaltMusic();
}
