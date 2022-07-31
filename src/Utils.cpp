#include "Utils.h"


void changeTextFromSurface(const string& newString, ResourceManager& resource, const int textResourceID) {
	TextAssets& textAssets = *(resource.textAssets);
	Text& highscoreText = textAssets[textResourceID];

	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(highscoreText.font, newString.c_str(), highscoreText.color);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(resource.renderer, surfaceMessage);

	SDL_FreeSurface(highscoreText.surface);
	SDL_DestroyTexture(highscoreText.texture);

	highscoreText.surface = surfaceMessage;
	highscoreText.texture = Message;
}