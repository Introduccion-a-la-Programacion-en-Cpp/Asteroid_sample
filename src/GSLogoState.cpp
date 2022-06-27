#include "GSLogoState.h"
#include "StructsDef.h"
#include "SDL.h"
#include "SDL_image.h"

// definimos y usamos namespace para poder volver a crear variables con el mismo nombre en cada cpp de los estados
namespace logo {
	// Podemos repetir el formato de manejo de estados para manejar sub-estados de forma independiente
	enum SUBSTATE {
		INIT_STATE = 0,
		LOGO_STATE,
		ENDING_STATE,
	};

	int subState = INIT_STATE;
	float timer = 0;
	const float LOGO_TIME = 5000.0f; // 5000 ms
	int logo_resourceID = -1;
}

using namespace logo;

void loadResource(ResourceManager& resource) {
	SDL_Renderer* renderer = resource.renderer;
	SpriteAssets& spritesAssets = *(resource.spritesAssets);

	// Cargo el Logo principal.
	string filePath = "assets/img/splash.png";
	SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
	SDL_Rect dest;
	dest.x = 0 ;
	dest.y = 0;
	dest.w = WIDTH;
	dest.h = HEIGHT;

	Sprite logoSprite;
	logoSprite.dest = dest;
	logoSprite.texture = texture;
	spritesAssets.push_back(logoSprite); // Se agrega al final del vector

	logo_resourceID = spritesAssets.size() - 1; // Entonces obtengo el indice del asset agregado, asi luego lo puedo usar o eliminar.
}

void unloadResource(ResourceManager& resource) {

	SpriteAssets& spritesAssets = *(resource.spritesAssets);
	
	//Libero la textura
	SDL_DestroyTexture(spritesAssets[logo_resourceID].texture);
	// Remuevo el asset del vector y ya no sera pintado nunca mas.
	spritesAssets.erase(spritesAssets.begin() + logo_resourceID); 
}


void GSLogoStateUpdate(float delta, ResourceManager& resource) {

	InputState& inputState = *resource.inputState;
	GameStages& gameStages = *resource.gameStages;

	switch (subState) {
	case INIT_STATE:
		loadResource(resource);
		subState = LOGO_STATE;
		break;

	case LOGO_STATE:
		timer += delta;
		if (timer >= LOGO_TIME) {
			subState = ENDING_STATE;
		}

		break;
	case ENDING_STATE:
		// Limpiamos recursos
		subState = INIT_STATE;
		unloadResource(resource);
		gameStages.pop(); // Removemos el estado splash o logo

		/// Y agregamos el estado Main Menu que sera actualizado en otro modulo
		 // Starting Game stage
		GameStage mainMenuGameStage;
		mainMenuGameStage.game_stageID = GS_MAIN_MENU;
		mainMenuGameStage.stage_name = "MainMenu";
		gameStages.push(mainMenuGameStage);
		
		break;
	}


}
