#include <string>
#include <queue>    
#include "GSLogoState.h"
#include "StructsDef.h"
#include "GamePlayStructsDef.h"
#include "Utils.h"
#include "SDL.h"
#include "SDL_image.h"
#include "MusicDef.h"

#define USE_DEBUG_TEXT


namespace gameplay {
	enum SUBSTATE {
		INIT_STATE = 0,
		UPDATE_STATE,
		ENDING_STATE,
	};

	const float SPACESHIP_ANGLE_SPEED = 0.50f;
	const unsigned int SPACESHIP_MAX_ANGLE = 360;
	const unsigned int SPACESHIP_MIN_ANGLE = 0;
	const float SPACESHIP_FLY_SPEED = .3f;
	const float SPACESHIP_FLY_MAX_SPEED = 1.0f;
	const float SPACECHIP_FLY_FRICTION_FACTOR = .003f;
	const int SPACESHIP_WIDTH = 30;
	const int SPACESHIP_HEIGHT = 30;
	const int SPACESHIP_AMMO_WIDTH = 3;
	const int SPACESHIP_AMMO_HEIGHT = SPACESHIP_HEIGHT;
	const int SPACESHIP_MAX_AMMO = 5;
	const int SPACESHIP_MAX_SPEED = 10;
	const int ASTEROID_WITDH = 45;
	const int ASTEROID_HEIGHT = 50;
	const int ASTEROID_MAX = 10;
	const float ASTEROID_RESPAWN_TIME = 5000.0f; // every second
	const float ASTEROID_SPEED = .001f;
	const float ASTEROID_MAX_SPEED = .02f;
	const float SPACECHIP_SHOOTCOOLDOWN = 100.f;
	const float SHOOTCOOLDOWN = 1200.f;

	int subState = INIT_STATE;
	float timer = 0;
	float asteroid_respawnTime = 0.0f;
	const float LOGO_TIME = 5000.0f; // 5000 ms
	int bg_resourceID = -1;
	int spaceship_resourceID = -1;
	int textHighScore_resourceID = -1;
	SpaceShipAmmo spaceship_ammo;
	SpaceShip spaceship;
	queue<int> ammo_available;

	EnemiesAsteroid allEnemies;

#ifdef USE_DEBUG_TEXT
	int debugText_resourceID = -1;
#endif
	unsigned int score = 0;

	SDL_Texture* createSpaceshipTexture(ResourceManager& resource) {
		SDL_Renderer* renderer = resource.renderer;

		SDL_Texture* texture_target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_TARGET, SPACESHIP_WIDTH, SPACESHIP_HEIGHT);
		//Set the target texture for rendering
		SDL_SetRenderTarget(renderer, texture_target);
		SDL_SetTextureBlendMode(texture_target, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 120, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		//Drawing the lines we want. (-1, 0), (0, 1), (1, 0)
		int line0 = SDL_RenderDrawLine(renderer, 0, SPACESHIP_HEIGHT, SPACESHIP_WIDTH >> 1, 0);
		int line1 = SDL_RenderDrawLine(renderer, SPACESHIP_WIDTH, SPACESHIP_HEIGHT, SPACESHIP_WIDTH >> 1, 0);
		int line2 = SDL_RenderDrawLine(renderer, 0, SPACESHIP_HEIGHT, SPACESHIP_WIDTH, SPACESHIP_HEIGHT);
		//Detach the target texture
		SDL_SetRenderTarget(renderer, NULL);
		//Save texture

		return texture_target;
	}

	SDL_Texture* createSpaceshipShoot(ResourceManager& resource) {
		SDL_Renderer* renderer = resource.renderer;

		SDL_Texture* texture_target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_TARGET, SPACESHIP_AMMO_WIDTH, SPACESHIP_AMMO_HEIGHT);
		//Set the target texture for rendering
		SDL_SetRenderTarget(renderer, texture_target);
		SDL_SetTextureBlendMode(texture_target, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 120, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		//Drawing the lines we want. (-1, 0), (0, 1), (1, 0)
		int line0 = SDL_RenderDrawLine(renderer, SPACESHIP_AMMO_WIDTH >> 1, SPACESHIP_AMMO_HEIGHT, SPACESHIP_AMMO_WIDTH >> 1, 0);
		
		//Glow effect
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
		int line1 = SDL_RenderDrawLine(renderer, (SPACESHIP_AMMO_WIDTH >> 1) + 1, SPACESHIP_AMMO_HEIGHT, (SPACESHIP_AMMO_WIDTH >> 1) + 1, 0);
		int line2 = SDL_RenderDrawLine(renderer, (SPACESHIP_AMMO_WIDTH >> 1) - 1, SPACESHIP_AMMO_HEIGHT, (SPACESHIP_AMMO_WIDTH >> 1) - 1, 0);
		//Detach the target texture
		SDL_SetRenderTarget(renderer, NULL);
		//Save texture

		return texture_target;
	}

	SDL_Texture* createAsteroid(ResourceManager& resource) {
		SDL_Renderer* renderer = resource.renderer;

		SDL_Texture* texture_target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_TARGET, ASTEROID_WITDH, ASTEROID_HEIGHT);
		//Set the target texture for rendering
		SDL_SetRenderTarget(renderer, texture_target);
		SDL_SetTextureBlendMode(texture_target, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 120, 0, 0, SDL_ALPHA_TRANSPARENT);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		
		//Drawing the lines we want. (-1, 0), (0, 1), (1, 0)
		/*   __A
		  B /  \ F
		  C \__/ E
		 D      */
		const int start_x = 0;
		const int start_y = 0;
		short
			ast_slant_width, // the width of B, C, E or F
			ast_slant_height, // the height of B, C, E or F
			ast_straight_width; // the width of A or D

		ast_slant_width = ASTEROID_HEIGHT >> 2;
		ast_slant_height = ASTEROID_HEIGHT >> 1;
		ast_straight_width = ASTEROID_WITDH >> 1;

		// A
		SDL_RenderDrawLine(renderer,
			start_x + ast_slant_width,
			start_y,
			start_x + ast_slant_width + ast_straight_width,
			start_y
		);

		// B
		SDL_RenderDrawLine(renderer,
			start_x + ast_slant_width,
			start_y,
			start_x,
			start_y + ast_slant_height
		);

		// C
		SDL_RenderDrawLine(renderer,
			start_x,
			start_y + ast_slant_height,
			start_x + ast_slant_width,
			start_y + (ast_slant_height * 2)
		);

		// D
		SDL_RenderDrawLine(renderer,
			start_x + ast_slant_width,
			start_y + (ast_slant_height << 1) - 1 ,
			start_x + ast_slant_width + ast_straight_width,
			start_y + (ast_slant_height << 1) -1
		);

		// E
		SDL_RenderDrawLine(renderer,
			start_x + ast_slant_width + ast_straight_width + 1,
			start_y + (ast_slant_height * 2),
			start_x + (ast_slant_width * 2) + ast_straight_width - 2,
			start_y + ast_slant_height +1
		);

		// F
		SDL_RenderDrawLine(renderer,
			start_x + (ast_slant_width * 2) + ast_straight_width ,
			start_y + ast_slant_height + 3,
			start_x + ast_slant_width + ast_straight_width + 1,
			start_y  
		);
	
		//Detach the target texture
		SDL_SetRenderTarget(renderer, NULL);
		//Save texture

		return texture_target;
	}

	
	void loadAssets(ResourceManager& resource) {
		SDL_Renderer* renderer = resource.renderer;
		SpriteAssets& spritesAssets = *(resource.spritesAssets);
		TextAssets& textAssets = *(resource.textAssets);

		// Cargo el Logo principal.
		string filePath = "assets/img/background_gameplay.png";
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

		// Generating the spaceship
		SDL_Texture* spaceshipTexture = createSpaceshipTexture(resource);
		SDL_Rect spaceship_dest;
		spaceship_dest.x = WIDTH >> 1;
		spaceship_dest.y = HEIGHT >> 1;
		spaceship_dest.w = SPACESHIP_WIDTH;
		spaceship_dest.h = SPACESHIP_HEIGHT;
		Sprite spaceshipSprite;
		spaceshipSprite.dest = spaceship_dest;
		spaceshipSprite.texture = spaceshipTexture;
		spaceshipSprite.angle = 0;
		spritesAssets.push_back(spaceshipSprite); // Se agrega al final del vector

		spaceship_resourceID = spritesAssets.size() - 1; // Entonces obtengo el indice del asset agregado, asi luego lo puedo usar o eliminar.

		// Creating Score and lifes...
		// Title
		// Main Menu options
		// Cargo el texto...
		string fontfilePath = "assets/fonts/arial.ttf";

		TTF_Font* Sans = TTF_OpenFont(fontfilePath.c_str(), 24); //this opens a font style and sets a size

		SDL_Color White = { 255, 255, 255 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "0000", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture
		
		SDL_Rect Message_rect; //create a rect		
		Message_rect.w = WIDTH * 0.09; // controls the width of the rect
		Message_rect.h = HEIGHT * 0.05; // controls the height of the rect
		Message_rect.x = (WIDTH >> 1) - (Message_rect.w >> 1);  //controls the rect's x coordinate 
		Message_rect.y = HEIGHT * 0.05; // controls the rect's y coordinte

		Text highscoreText;
		highscoreText.font = Sans;
		highscoreText.color = White;
		highscoreText.surface = surfaceMessage;
		highscoreText.texture = Message;
		highscoreText.dest = Message_rect;

		textAssets.push_back(highscoreText);

		textHighScore_resourceID = textAssets.size() - 1;

		// Generate Asteroids...

		SDL_Texture* asteroidTexture = createAsteroid(resource);
		SDL_Rect asteroid_dest;
		asteroid_dest.x = 0;
		asteroid_dest.y = 0;
		asteroid_dest.w = ASTEROID_WITDH;
		asteroid_dest.h = ASTEROID_HEIGHT;
		Sprite asteroid_destSprite;
		asteroid_destSprite.dest = asteroid_dest;
		asteroid_destSprite.texture = asteroidTexture;
		asteroid_destSprite.angle = 0;
		
		srand(time(NULL));
		
		int initial_asteroid_index = spritesAssets.size();
		
		// first I create the sprites...
		for (int i = 0; i < ASTEROID_MAX; i++) {
			asteroid_destSprite.dest.x = rand() % (WIDTH );
			asteroid_destSprite.dest.y = rand() % (HEIGHT );
			spritesAssets.push_back(asteroid_destSprite); // Se agrega al final del vector	
		}

		//Preparing Ammo.
		spaceship_ammo.reserve(SPACESHIP_MAX_AMMO);

		SDL_Texture* ammoTexture = createSpaceshipShoot(resource);
		SDL_Rect ammo_dest;
		ammo_dest.x = WIDTH >> 1;
		ammo_dest.y = HEIGHT >> 1;
		ammo_dest.w = SPACESHIP_AMMO_WIDTH;
		ammo_dest.h = SPACESHIP_AMMO_HEIGHT;
		Sprite ammo_destSprite;
		ammo_destSprite.dest = ammo_dest;
		ammo_destSprite.texture = ammoTexture;
		ammo_destSprite.angle = 0;
		
		int initial_ammo_index = spritesAssets.size();

		// first I create the sprites...
		for (int i = 0; i < SPACESHIP_MAX_AMMO; i++) {
			spritesAssets.push_back(ammo_destSprite); // Se agrega al final del vector	
		}

		// Then I create the ammo... be aware that you need to wait until all the sprites has been created at asset vectos
		for (int i = initial_ammo_index; i < (initial_ammo_index + SPACESHIP_MAX_AMMO); i++) {
			Ammo spaceshipAmmo;
			spaceshipAmmo.sprite = &spritesAssets[i];
			spaceshipAmmo.sprite->isVisible = false;
			spaceship_ammo.push_back(spaceshipAmmo);
			ammo_available.push(spaceship_ammo.size() - 1);
		}


		for (int i = initial_asteroid_index; i < (initial_asteroid_index + ASTEROID_MAX); i++) {
			Asteroid asteroid;
			asteroid.sprite = &spritesAssets[i];
			//asteroid.sprite->isVisible = false;
			asteroid.scale = (rand() % 100) / 100.0f;
			asteroid.currentSpaceshipVelocity = ASTEROID_SPEED;
			int random_number = rand();
			asteroid.currentSpaceshipDir.x = ((rand() % 2 )? -1 : 1) * (random_number % (WIDTH)) * 1.f  / (WIDTH * 10) ;
			asteroid.currentSpaceshipDir.y = ((rand() % 2) ? -1 : 1) * (random_number % (HEIGHT)) * 1.f / (HEIGHT * 10);
			allEnemies.push_back(asteroid);
		}
		
		
		// Assign the references
		spaceship.sprite = &spritesAssets[spaceship_resourceID];
		spaceship.shoot_cooldown = SPACECHIP_SHOOTCOOLDOWN;

		// The following lines are only available if USE_DEBUG_TEXT is defined... only for testing purposes
#ifdef USE_DEBUG_TEXT
		surfaceMessage = TTF_RenderText_Solid(Sans, "0000.0", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

		Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture

		SDL_Rect Debug_Message_rect; //create a rect		
		Debug_Message_rect.w = WIDTH * 0.09; // controls the width of the rect
		Debug_Message_rect.h = HEIGHT * 0.05; // controls the height of the rect
		Debug_Message_rect.x = (WIDTH >> 1) - (Message_rect.w >> 1);  //controls the rect's x coordinate 
		Debug_Message_rect.y = HEIGHT * 0.85; // controls the rect's y coordinte
		Text debugText;
		debugText.font = Sans;
		debugText.color = White;
		debugText.surface = surfaceMessage;
		debugText.texture = Message;
		debugText.dest = Debug_Message_rect;
		textAssets.push_back(debugText);
		debugText_resourceID = textAssets.size() - 1;
#endif // USE_DEBUG_TEXT		
	}

	void unloadAssets(ResourceManager& resource) {
		SpriteAssets& spritesAssets = *(resource.spritesAssets);
		TextAssets& textAssets = *(resource.textAssets);
		Text& highscoreText = textAssets[textHighScore_resourceID];

		//Libero la textura
		SDL_DestroyTexture(spritesAssets[bg_resourceID].texture);
		SDL_DestroyTexture(spritesAssets[spaceship_resourceID].texture);
#ifdef USE_DEBUG_TEXT
		SDL_DestroyTexture(textAssets[debugText_resourceID].texture);
		SDL_FreeSurface(textAssets[debugText_resourceID].surface);
		textAssets.erase(textAssets.begin() + debugText_resourceID);
#endif

		// Remuevo el asset del vector y ya no sera pintado nunca mas.
		
		spritesAssets.erase(spritesAssets.begin() + spaceship_resourceID);
		spritesAssets.erase(spritesAssets.begin() + bg_resourceID);

		// Free text
		SDL_DestroyTexture(highscoreText.texture);
		SDL_FreeSurface(highscoreText.surface);
		textAssets.erase(textAssets.begin() + textHighScore_resourceID);
		TTF_CloseFont(highscoreText.font);
	}

	void updateAmmo(float delta) {
		for (int i = 0; i < SPACESHIP_MAX_AMMO; i++) {
			Ammo& spaceshipAmmo = spaceship_ammo[i];

			if (spaceshipAmmo.sprite->isVisible) {

				spaceshipAmmo.shoot_cooldown -= delta;

				// Clamping values
				if (spaceshipAmmo.currentSpaceshipVelocity < 0) {
					spaceshipAmmo.currentSpaceshipVelocity = 0.0f;
				}

				if (spaceshipAmmo.currentSpaceshipVelocity > SPACESHIP_FLY_MAX_SPEED) {
					spaceshipAmmo.currentSpaceshipVelocity = SPACESHIP_FLY_MAX_SPEED;
				}

				if (spaceshipAmmo.currentSpaceshipVelocity > 0.0f) {
					spaceshipAmmo.sprite->dest.x += spaceshipAmmo.currentSpaceshipDir.x * delta;
					spaceshipAmmo.sprite->dest.y += spaceshipAmmo.currentSpaceshipDir.y * delta;
				}

				if (spaceshipAmmo.shoot_cooldown <= 0.0f) {
					spaceshipAmmo.sprite->isVisible = false;
					spaceshipAmmo.shoot_cooldown = 0.f;
					ammo_available.push(i);
				}

				// Repost spaceship if out of scren

				if (spaceshipAmmo.sprite->dest.x < 0) {
					spaceshipAmmo.sprite->dest.x = WIDTH - SPACESHIP_WIDTH;
				}
				else if (spaceshipAmmo.sprite->dest.x > (WIDTH + SPACESHIP_WIDTH)) {
					spaceshipAmmo.sprite->dest.x = 0;
				}

				if (spaceshipAmmo.sprite->dest.y < 0) {
					spaceshipAmmo.sprite->dest.y = HEIGHT - SPACESHIP_HEIGHT;
				}
				else if (spaceshipAmmo.sprite->dest.y > (HEIGHT + SPACESHIP_HEIGHT)) {
					spaceshipAmmo.sprite->dest.y = 0;
				}

			}
		}
	}

	void updateAsteroid(float delta) {
		asteroid_respawnTime -= delta;
		for (int i = 0; i < ASTEROID_MAX; i++) {
			Asteroid& asteroid = allEnemies[i];

			if (asteroid.sprite->isVisible) {

				// Clamping values
				if (asteroid.currentSpaceshipVelocity < 0) {
					asteroid.currentSpaceshipVelocity = 0.0f;
				}

				if (asteroid.currentSpaceshipVelocity > ASTEROID_MAX_SPEED) {
					asteroid.currentSpaceshipVelocity = ASTEROID_MAX_SPEED;
				}

				if (asteroid.currentSpaceshipVelocity > 0.0f) {
					asteroid.sprite->dest.x += asteroid.currentSpaceshipDir.x * delta;
					asteroid.sprite->dest.y += asteroid.currentSpaceshipDir.y * delta;
				}


				// Repost spaceship if out of scren

				if (asteroid.sprite->dest.x < 0) {
					asteroid.sprite->dest.x = WIDTH - ASTEROID_WITDH;
				}
				else if (asteroid.sprite->dest.x > (WIDTH + ASTEROID_WITDH)) {
					asteroid.sprite->dest.x = 0;
				}

				if (asteroid.sprite->dest.y < 0) {
					asteroid.sprite->dest.y = HEIGHT - ASTEROID_HEIGHT;
				}
				else if (asteroid.sprite->dest.y > (HEIGHT + ASTEROID_HEIGHT)) {
					asteroid.sprite->dest.y = 0;
				}

			}
			else if(asteroid_respawnTime < 0.0f) { // if NOT visible and respawn wait time < 0 then repost asteriod
				asteroid.sprite->isVisible = true;
 				int randomX = (rand() % 2) ? 0 - (WIDTH >> 2) : WIDTH + (WIDTH >> 2);
				int randomY = (rand() % 2) ? 0 - (HEIGHT >> 2) : HEIGHT + (HEIGHT >> 2);
				asteroid.sprite->dest.x += randomX ; 
				asteroid.sprite->dest.y += randomY ;
				// May it would be funny if we get the x and y platyer pos and send asteroid there dir..
				asteroid_respawnTime = ASTEROID_RESPAWN_TIME; // we will wait unitl next round
			}
		}

		// Removed... only reset if we have non-visible Asteroids
		/*if (asteroid_respawnTime < 0.0f) {
			asteroid_respawnTime = ASTEROID_RESPAWN_TIME;
		}*/
	}

	void updateSpaceShip(float delta, ResourceManager& resource) {
		InputState& inputState = *resource.inputState;
		
		if (inputState.left) {
			spaceship.sprite->angle -= SPACESHIP_ANGLE_SPEED * delta;
		}
		else if (inputState.right) {
			spaceship.sprite->angle += SPACESHIP_ANGLE_SPEED * delta;
		}

		
		// TODO: Only calculate when angle changed...
		float angle = spaceship.sprite->angle - 90 ; //Adding an offset due to initial sprite is pointing up
		float radians = (float)(angle * M_PI) / 180.0f;
		float cosAngle = SDL_cosf(radians);
		float sinAngle = SDL_sinf(radians);
		
		if (inputState.up) {
			spaceship.currentSpaceshipVelocity += SPACESHIP_FLY_SPEED;
		}
		else if (inputState.down) {
			spaceship.currentSpaceshipVelocity -= SPACESHIP_FLY_SPEED;
		}

		// Clamping values
		if (spaceship.currentSpaceshipVelocity < 0) {
			spaceship.currentSpaceshipVelocity = 0.0f;
		}

		if (spaceship.currentSpaceshipVelocity > SPACESHIP_FLY_MAX_SPEED) {
			spaceship.currentSpaceshipVelocity = SPACESHIP_FLY_MAX_SPEED;
		}

		if (spaceship.sprite->angle > SPACESHIP_MAX_ANGLE) {
			spaceship.sprite->angle = SPACESHIP_MIN_ANGLE;
		}

		if (spaceship.sprite->angle < SPACESHIP_MIN_ANGLE) {
			spaceship.sprite->angle = SPACESHIP_MAX_ANGLE;
		}

		if (spaceship.shoot_cooldown <= 0.f) {
			spaceship.shoot_cooldown = 0.f;
		}
		// Clamping values

		if (inputState.right || inputState.left) {
			spaceship.currentSpaceshipDir.x = (1 + spaceship.currentSpaceshipVelocity) * cosAngle;
			spaceship.currentSpaceshipDir.y = (1 + spaceship.currentSpaceshipVelocity) * sinAngle;
		}

		if (spaceship.currentSpaceshipVelocity > 0.0f) {

			spaceship.sprite->dest.x += spaceship.currentSpaceshipDir.x * delta;
			spaceship.sprite->dest.y += spaceship.currentSpaceshipDir.y * delta;

			// Inertia movement..
			if (!inputState.up && !inputState.down) {
				spaceship.currentSpaceshipVelocity -= SPACECHIP_FLY_FRICTION_FACTOR * delta;
			}
		}

		if (spaceship.shoot_cooldown > 0.f) {
			spaceship.shoot_cooldown -= delta;
		}

		// Repost spaceship if out of scren
		
		if (spaceship.sprite->dest.x < 0) {
			spaceship.sprite->dest.x = WIDTH - SPACESHIP_WIDTH;
		}
		else if (spaceship.sprite->dest.x > (WIDTH + SPACESHIP_WIDTH) ) {
			spaceship.sprite->dest.x = 0;
		}

		if (spaceship.sprite->dest.y < 0) {
			spaceship.sprite->dest.y = HEIGHT - SPACESHIP_HEIGHT;
		}
		else if (spaceship.sprite->dest.y > (HEIGHT + SPACESHIP_HEIGHT)) {
			spaceship.sprite->dest.y = 0;
		}

		if (inputState.fire && !ammo_available.empty() && (spaceship.shoot_cooldown <= 0.f) ) {
			spaceship.shoot_cooldown = SPACECHIP_SHOOTCOOLDOWN;
			Ammo& shoot = spaceship_ammo[ammo_available.front()];
			shoot.sprite->isVisible = true;
			shoot.shoot_cooldown = SHOOTCOOLDOWN;
			shoot.sprite->angle = spaceship.sprite->angle;
			shoot.sprite->dest.x = spaceship.sprite->dest.x;
			shoot.sprite->dest.y = spaceship.sprite->dest.y;
			shoot.currentSpaceshipVelocity = spaceship.currentSpaceshipVelocity + SPACESHIP_FLY_SPEED;
			shoot.currentSpaceshipDir.x = spaceship.currentSpaceshipDir.x;
			shoot.currentSpaceshipDir.y = spaceship.currentSpaceshipDir.y;
			ammo_available.pop();
		}

		// Update HighScore
		changeTextFromSurface(std::to_string(spaceship.sprite->angle), resource, textHighScore_resourceID);
#ifdef USE_DEBUG_TEXT
		// update debug text with speed value..
		changeTextFromSurface(std::to_string(spaceship.currentSpaceshipVelocity), resource, debugText_resourceID);
#endif 
	}

	void updateCheckCollision() {

		const SDL_Rect* playerRect = &spaceship.sprite->dest;
		// Check any ammo with any asteroid. a list of N elements against a list of M elements.
		// amount of comparision N ^ M... a complex function. a proper optimization it would be
		// have reduced list of elements but the worst case scenario it would be N ^ M anyway
		for (size_t i = 0; i < ASTEROID_MAX; i++) {
			Asteroid enemy = allEnemies[i];
			bool hitByAmmo = false;
			bool hitPlayer = false;
			const SDL_Rect* enemyRect = &enemy.sprite->dest;

			if (enemy.sprite->isVisible) { // this check will reduce the amount of comparissions
				for (size_t i = 0; i < SPACESHIP_MAX_AMMO; i++) { // sadly we must check against all the shoots in the screen
					Ammo& spaceshipAmmo = spaceship_ammo[i];

					if (spaceshipAmmo.sprite->isVisible) { //This will reduce the amount of comparission as well
						
						const SDL_Rect* shootRect = &spaceshipAmmo.sprite->dest;
						hitByAmmo = SDL_HasIntersection(enemyRect, shootRect);

						if (hitByAmmo) { // if hits happens... then break the loop and remove the shoot
							spaceshipAmmo.sprite->isVisible = false;
							spaceshipAmmo.shoot_cooldown = 0.f;
							ammo_available.push(i);							
							break;
						}
					}
				}

				hitPlayer = SDL_HasIntersection(enemyRect, playerRect);
				if (hitPlayer) {
					//TODO: repost player position.. and reduce 1 life

					spaceship.sprite->dest.x = WIDTH >> 1;
					spaceship.sprite->dest.y = HEIGHT >> 1;
					spaceship.currentSpaceshipVelocity = 0.0f;
				}
			}

			if (hitByAmmo) { //if hit by ammo then blow up (or hide)
				enemy.sprite->isVisible = false;
			}

		}
	}

}

using namespace gameplay;

void GSGameplayStateUpdate(float delta, ResourceManager& resource) {

	SDL_Renderer* renderer = resource.renderer;

	switch (subState)
	{
	case INIT_STATE:
		loadAssets(resource);
		subState = UPDATE_STATE;
#ifdef USE_MUSIC
		PlayBGM(BGM_GAMEPLAY, resource);
#endif // USE_MUSIC

		break;
	case UPDATE_STATE:
		updateSpaceShip(delta, resource);
		updateAmmo(delta);
		updateAsteroid(delta);
		updateCheckCollision();
		break;
	case ENDING_STATE:
		unloadAssets(resource);
		subState = -1;
		break;
	default:
		break;
	}

}