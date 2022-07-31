#include "GSMainMenuState.h"
#include "StructsDef.h"
#include "SDL.h"
#include "SDL_image.h"
#include "MusicDef.h"


namespace mainmenu {
	// Podemos repetir el formato de manejo de estados para manejar sub-estados de forma independiente
	enum SUBSTATE {
		INIT_STATE = 0,
		PRESSKEY_STATE,
		MAINMENU_STATE,
		ENDING_STATE,
	};

	enum MENUOPTIONS {
		PLAY = 0,
		HIGHSCORES,
		MENUITEM_COUNT,
	};

	int subState = INIT_STATE;
	int bg_resourceID = -1;
	// Para ser usado en distintos contadores..
	float timer = 1.0f * 1000; // 1000 ms
	int menu_selection = PLAY;
	int menuselector_resourceID = -1;

	void loadResource(ResourceManager& resource) {
		SDL_Renderer* renderer = resource.renderer;
		SpriteAssets& spritesAssets = *(resource.spritesAssets);
		TextAssets& textAssets = *(resource.textAssets);

		// Cargo el Logo principal.
		string filePath = "assets/img/bg.png";
		SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
		SDL_Rect dest;
		dest.x = 0;
		dest.y = 0;
		dest.w = WIDTH;
		dest.h = HEIGHT;

		Sprite bgSprite;
		bgSprite.dest = dest;
		bgSprite.texture = texture;
		spritesAssets.push_back(bgSprite); // Se agrega al final del vector

		bg_resourceID = spritesAssets.size() - 1; // Entonces obtengo el indice del asset agregado, asi luego lo puedo usar o eliminar.

		// Cargo el texto...
		string fontfilePath = "assets/fonts/arial.ttf";

		TTF_Font* Sans = TTF_OpenFont(fontfilePath.c_str(), 24); //this opens a font style and sets a size

		SDL_Color White = { 255, 255, 255 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "Press any key to start...", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture

		SDL_Rect Message_rect; //create a rect
		Message_rect.w = WIDTH * 0.65; // controls the width of the rect
		Message_rect.h = HEIGHT * 0.10; // controls the height of the rect
		Message_rect.x = (WIDTH >> 1) - (Message_rect.w >> 1);  //controls the rect's x coordinate 
		Message_rect.y = HEIGHT *0.85; // controls the rect's y coordinte

		Text mainText;
		mainText.font = Sans;
		mainText.color = White;
		mainText.surface = surfaceMessage;
		mainText.texture = Message;
		mainText.dest = Message_rect;

		textAssets.push_back(mainText);

		// Main Menu options
		surfaceMessage = TTF_RenderText_Solid(Sans, "Start game", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

		Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture

		Message_rect.w = WIDTH * 0.15; // controls the width of the rect
		Message_rect.h = HEIGHT * 0.05; // controls the height of the rect
		Message_rect.x = (WIDTH >> 1) - (Message_rect.w >> 1);  //controls the rect's x coordinate 
		Message_rect.y = HEIGHT >> 1; // controls the rect's y coordinte

		Text mainMenuText;
		mainMenuText.font = Sans;
		mainMenuText.color = White;
		mainMenuText.surface = surfaceMessage;
		mainMenuText.texture = Message;
		mainMenuText.dest = Message_rect;
		mainMenuText.isVisible = false;

		textAssets.push_back(mainMenuText);


		surfaceMessage = TTF_RenderText_Solid(Sans, "High Scores", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

		Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture

		Message_rect.w = WIDTH * 0.15; // controls the width of the rect
		Message_rect.h = HEIGHT * 0.05; // controls the height of the rect
		Message_rect.x = (WIDTH >> 1) - (Message_rect.w >> 1);  //controls the rect's x coordinate 
		Message_rect.y = (HEIGHT >> 1) + Message_rect.h + 10; // controls the rect's y coordinte

		Text highScoresText;
		highScoresText.font = Sans;
		highScoresText.color = White;
		highScoresText.surface = surfaceMessage;
		highScoresText.texture = Message;
		highScoresText.dest = Message_rect;
		highScoresText.isVisible = false;

		textAssets.push_back(highScoresText);


		// selector options
		surfaceMessage = TTF_RenderText_Solid(Sans, ">           <", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

		Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture

		Message_rect.w = WIDTH * 0.25; // controls the width of the rect
		Message_rect.h = HEIGHT * 0.05; // controls the height of the rect
		Message_rect.x = (WIDTH >> 1) - (Message_rect.w >> 1);  //controls the rect's x coordinate 
		Message_rect.y = HEIGHT >> 1; // controls the rect's y coordinte

		Text menuSelectorText;
		menuSelectorText.font = Sans;
		menuSelectorText.color = White;
		menuSelectorText.surface = surfaceMessage;
		menuSelectorText.texture = Message;
		menuSelectorText.dest = Message_rect;
		menuSelectorText.isVisible = false;

		textAssets.push_back(menuSelectorText);

		menuselector_resourceID = textAssets.size() - 1;

		// Title
		// Main Menu options
		surfaceMessage = TTF_RenderText_Solid(Sans, "Asteroid", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

		Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture

		Message_rect.w = WIDTH * 0.75; // controls the width of the rect
		Message_rect.h = HEIGHT * 0.25; // controls the height of the rect
		Message_rect.x = (WIDTH >> 1) - (Message_rect.w >> 1);  //controls the rect's x coordinate 
		Message_rect.y = HEIGHT >> 4; // controls the rect's y coordinte

		Text titleText;
		titleText.font = Sans;
		titleText.color = White;
		titleText.surface = surfaceMessage;
		titleText.texture = Message;
		titleText.dest = Message_rect;

		textAssets.push_back(titleText);
	}

	void unloadResource(ResourceManager& resource) {

		SpriteAssets& spritesAssets = *(resource.spritesAssets);
		TextAssets& textAssets = *(resource.textAssets);

		//Libero la textura
		SDL_DestroyTexture(spritesAssets[bg_resourceID].texture);
		// Remuevo el asset del vector y ya no sera pintado nunca mas.
		spritesAssets.erase(spritesAssets.begin() + bg_resourceID);

		for (int i = 0; i < textAssets.size(); i++) {
			SDL_FreeSurface(textAssets[i].surface);
			SDL_DestroyTexture(textAssets[i].texture);
		}

		textAssets.clear();
	}
}

using namespace mainmenu;


void GSMainMenuStateUpdate(float delta, ResourceManager& resource) {
	InputState& inputState = *resource.inputState;
	GameStages& gameStages = *resource.gameStages;
	TextAssets& textAssets = *resource.textAssets;

	const float BLINK_SPEED = 5.0f;

	timer -= BLINK_SPEED * delta;

	Text titleText;
	int yOffset = 0; 

	switch (subState) {
	case INIT_STATE:
		loadResource(resource);
		subState = PRESSKEY_STATE;
		PlayBGM(BGM_MAIN_MENU, resource);
		break;
	case PRESSKEY_STATE:
		// para efecto de parpadeo...
		if (timer <= 0.0f) {
			timer = 1.0f * 1000;
			textAssets[0].isVisible = !textAssets[0].isVisible;
		}

		if (inputState.fire || inputState.down || inputState.up || inputState.left || inputState.right ) {
			subState = MAINMENU_STATE;
			textAssets[0].isVisible = false;
			textAssets[1].isVisible = true;
			textAssets[2].isVisible = true;
			textAssets[3].isVisible = true;
			SDL_Delay(1500); //Esperamos 1.5 segundos...
		}


		break;
	case MAINMENU_STATE:
		// TODO: move cursor and move to next state

		if (inputState.up) {
			menu_selection--;
		}
		else if (inputState.down) {
			menu_selection++;
		}

		if (menu_selection < 0) {
			menu_selection = 0; // clamp on 0 as minimum option
		}

		if (menu_selection >= MENUITEM_COUNT) {
			menu_selection = MENUITEM_COUNT - 1; // clamp on highest menu option as maximum avaialable
		}

		// Now update the position
		titleText = textAssets[menuselector_resourceID];
		yOffset = (HEIGHT >> 1) + ( (titleText.dest.h + 10 ) * menu_selection ); // controls the rect's y coordinte
		textAssets[menuselector_resourceID].dest.y = yOffset;

		if (inputState.fire) {
			subState = ENDING_STATE;
		}

		break;
	case ENDING_STATE:
		// Limpiamos recursos
		subState = INIT_STATE;
		unloadResource(resource);
		StopBGM();
		//En este caso No removemos el estado MainMenu del stack...

		/// Y agregamos el estado Main Menu que sera actualizado en otro modulo
		 // Starting Game stage
		GameStage mainMenuGameStage;
		mainMenuGameStage.game_stageID = GS_GAMEPLAY;
		mainMenuGameStage.stage_name = "Gameplay";
		gameStages.push(mainMenuGameStage);

		break;
	}

}