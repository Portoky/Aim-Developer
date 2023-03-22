#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include "getpath.h"
#include "Header.h"
#include <time.h>
#include <cstdlib>
#include <sstream>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1017;

SDL_Window* win = NULL;
extern SDL_Renderer* ren;
extern TTF_Font* gFont;
LTexture LTetris, LEnemy[TOTAL_ENEMY_BUTTONS], LHeart, LNoHeart, LDeath, LPointTexture, LToxic1;

//Toxic players' voice lines
Mix_Chunk* mToxic1 = NULL;
Mix_Chunk* mShoot = NULL;


bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		logerror(std::cout, "SDL_Init()", E_SDL);
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		logerror(std::cout, "SDL_SetHint()", E_SDL);
		return false;
	}

	win = SDL_CreateWindow("Aim Developer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (win == NULL)
	{
		logerror(std::cout, "SDL_CreateWindow()", E_SDL);
		return false;
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL)
	{
		logerror(std::cout, "SDL_CreateRenderer()", E_SDL);
		return false;
	}
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	
	if (!Mix_Init(MIX_INIT_MP3))
	{
		logerror(std::cout, "Mix_Init", E_MIX);
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
	{
		logerror(std::cout, "Mix_OpenAudio", E_MIX);
		return false;
	}

	int ImgFlags = IMG_INIT_JPG;
	if (IMG_Init(ImgFlags) & ImgFlags == 0)
	{
		logerror(std::cout, "IMG_Init()", E_IMG);
		return false;
	}
	if (TTF_Init() == -1)
	{
		logerror(std::cout, "TTF_Init()", E_TTF);
		return false;
	}

	ImgFlags = IMG_INIT_PNG;
	if (IMG_Init(ImgFlags) & ImgFlags == 0)
	{
		logerror(std::cout, "IMG_Init()", E_IMG);
		return false;
	}
	if (TTF_Init() == -1)
	{
		logerror(std::cout, "TTF_Init()", E_TTF);
		return false;
	}

	return true;
}

bool loadMedia()
{
	//loading the voicelines
	mToxic1 = Mix_LoadWAV(getResourcePath("VoiceLines\\Voiceline01.wav").c_str());
	if (mToxic1 == NULL)
	{
		logerror(std::cout, "Mix_LoadWAV", E_MIX);
		return false;
	}

	mShoot = Mix_LoadWAV(getResourcePath("VoiceLines\\shoot_1.wav").c_str());
	if (mShoot == NULL)
	{
		logerror(std::cout, "Mix_LoadWAV", E_MIX);
		return false;
	}

	//loading the font
	gFont = TTF_OpenFont((getResourcePath("PointSystem.ttf")).c_str(), 28);
	if (gFont == NULL)
	{
		logerror(std::cout, "TTF_OpenFont", E_TTF);
		return false;
	}

	//loading textures

	if (!LToxic1.loadFromFile(getResourcePath("Textures\\Voiceline1.jpg")))
	{
		std::cout << "loadFromFile ERROR" << std::endl;
		return false;
	}

	if (!LTetris.loadFromFile(getResourcePath("Textures\\Tetris.jpg")))
	{
		std::cout << "loadFromFile ERROR" << std::endl;
		return false;
	}

	if (!LHeart.loadFromFile(getResourcePath("Textures\\heart.png")))
	{
		std::cout << "loadFromFile ERROR" << std::endl;
		return false;
	}

	if (!LNoHeart.loadFromFile(getResourcePath("Textures\\no_heart.png")))
	{
		std::cout << "loadFromFile ERROR" << std::endl;
		return false;
	}

	if (!LDeath.loadFromFile(getResourcePath("Textures\\death.jpg")))
	{
		std::cout << "loadFromFile ERROR" << std::endl;
		return false;
	}

	for (int j = 0; j < TOTAL_ENEMY_BUTTONS; ++j)
	{
		if (!LEnemy[j].loadFromFile(getResourcePath("Textures\\Enemy02.png")))
		{
			std::cout << "loadFromFile ERROR" << std::endl;
			return false;
		}
		LEnemy[j].setBlendMode(SDL_BLENDMODE_BLEND); //engedelyezzuk a blendelest az ellenfelek megjelenesenel
	}

	//placing enemies
	int spot = 0;//palace
	LEnemy[spot].setCenter({ 170, 301 }); //ami korul forog 
	LEnemy[spot].setFraction({ 70, 60, 300, 310 }); //mennyi kell a kepbol, az elso ket bal felso sarka a levagasnak
	LEnemy[spot].setAngle(13.0);// dolesszoge mekkora legyen
	LEnemy[spot].setPosition(250, 60);//hol legyen a kep
	LEnemy[spot].setMeasure(LEnemy[spot].getWidth() - 100, LEnemy[spot].getHeight() - 350); //mekkora legyen a kep;

	++spot;	//under palace
	LEnemy[spot].setCenter({ 170, 301 });
	LEnemy[spot].setFraction({ 0, 0, 340, 200 });
	LEnemy[spot].setPosition(450, 375);
	LEnemy[spot].setMeasure(LEnemy[spot].getWidth(), LEnemy[spot].getHeight() - 400);
	
	++spot;	//sandwich
	LEnemy[spot].setCenter({ 170, 301 });
	LEnemy[spot].setPosition(1450, 450);
	LEnemy[spot].setMeasure(LEnemy[spot].getWidth() - 200, LEnemy[spot].getHeight() - 350);
	LEnemy[spot].setColor(100, 100, 100);
	
	++spot;	//stairs
	LEnemy[spot].setCenter({ 170, 301 });
	LEnemy[spot].setPosition(1450, 180);
	LEnemy[spot].setFraction({ 0, 0, 340, 570 });
	LEnemy[spot].setAngle(-2.0);
	LEnemy[spot].setMeasure(LEnemy[spot].getWidth() - 200, LEnemy[spot].getHeight() - 350);

	++spot;	//site, behind the box
	LEnemy[spot].setCenter({ 170, 301 });
	LEnemy[spot].setPosition(1070, 520);
	LEnemy[spot].setFraction({ 0, 0, 200, 603 });
	LEnemy[spot].setMeasure(LEnemy[spot].getWidth() - 310, LEnemy[spot].getHeight() - 500);
	LEnemy[spot].setColor(225, 225, 225);

	++spot;	//middle of the site
	LEnemy[spot].setCenter({ 170, 301 });
	LEnemy[spot].setPosition(900, 525);
	LEnemy[spot].setMeasure(LEnemy[spot].getWidth() - 295, LEnemy[spot].getHeight() - 510);
	LEnemy[spot].setColor(250, 250, 250);
	
	++spot; //ct
	LEnemy[spot].setCenter({ 170, 301 });
	LEnemy[spot].setPosition(930, 525);
	LEnemy[spot].setMeasure(LEnemy[spot].getWidth() - 300, LEnemy[spot].getHeight() - 570);
	LEnemy[spot].setColor(250, 250, 250);
	LEnemy[spot].setFraction({ 0, 0, 340, 210 });
	LEnemy[spot].setColor(200, 200, 200);

	++spot;	//above check-in desk
	LEnemy[spot].setCenter({ 170, 301 });
	LEnemy[spot].setPosition(870, 460);
	LEnemy[spot].setMeasure(LEnemy[spot].getWidth() - 300, LEnemy[spot].getHeight() - 540);
	LEnemy[spot].setColor(250, 250, 250);
	LEnemy[spot].setFraction({ 0, 0, 340, 570 });

	//placing voicechat
	LToxic1.setPosition(50, 900);

	//setting a cursor with SDL_CreateSystemCursor
	SDL_Cursor* cursor;
	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	if (cursor == NULL)
	{
		logerror(std::cout, "SDL_CreateSystemCursor", E_SDL);
		return false;
	}
	SDL_SetCursor(cursor);
	return true;
}

void close()
{
	LTetris.free();
	LHeart.free();
	LNoHeart.free();

	for (int j = 0; j < TOTAL_ENEMY_BUTTONS; ++j)
	{
		LEnemy[j].free();
	}

	TTF_CloseFont(gFont);
	gFont = NULL;

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	ren = NULL;
	win = NULL;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main()
{
	srand(time(0));
	if (init())
	{
		if (loadMedia())
		{
			SDL_Event e;
			bool quit = false;
			Uint8 spot = 8;	//hol lesz az ellenfel 8 spot van
			bool enemy = false;	//van-e ellenseg a palyan, ha nincs ne tegyen tobbet
			bool shot = false; //meglottuk-e a vizsgalt ellenesegt
			//Uint8 i = 0;	//fokozatosan megjelennek az ellenfelek (ne egybol csak ugy odaspawnoljanak)
			Uint32 Startime;	//az idot merjuk vele (pl kell a timegap-nel)
			Uint8 lifes = 3;	//eletek szama
			Uint8 speed = 15; //7 lovesenkent no 20-al
			Uint16 kills = 0; //olesek szama
			Uint16 timeGap = 6000; //mennyi idot kapunk arra, hogy lelojuk az ellenfelelt
			bool talalt = false; //ha eltalalta barmelyik ellenfelet nem von le pontot, ezt ellenorzi ez
			std::stringstream PointText; //kiirassuk az oleseket / pontokat
			bool voicechat = false; //ha van voicechat, jelenitse meg a bal also sarokban
			int NumOfEnemies = 0;
			int NumOfEnemiesReady = 0;
			int TotalNumOfEnemies = 1;
			int shoti = -1; //ezt hasznaljuk arra, hogy meghatarozzuk melyiket lottek le
			int shotj = -1;
			while (!quit)
			{
				while (SDL_PollEvent(&e))
				{
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					if (e.type == SDL_MOUSEBUTTONDOWN && lifes)
					{
						//playing shooting sounds
						Mix_PlayChannel(-1, mShoot, 0);
						//eltalalta?
						talalt = false;
						for (int j = 0; j < TOTAL_ENEMY_BUTTONS; ++j)
						{
							if (LEnemy[j].alive && !LEnemy[j].shot || LEnemy[j].xHealth)
							{
								LEnemy[j].shot = LEnemy[j].handleEvent(&e);
								if (!talalt && LEnemy[j].shot)
								{
									talalt = true;
								}
							}
							else if (LEnemy[j].alive && LEnemy[j].shot)
							{
								talalt = true;
							}
							
						}
						shot = talalt;
				
						if (!talalt)
						{
							lifes--;
						}

						//playing voicechat lines
						if (rand() % 100 > 96 && Mix_Playing(-1))
						{
							Mix_PlayChannel(-1, mToxic1, 0);
							voicechat = true;

						}
						else if( Mix_Playing(-1) == 0)
							voicechat = false;
					}
				}

				if (lifes)
				{
					if (NumOfEnemiesReady == 0)
					{
						while (NumOfEnemies < TotalNumOfEnemies)
						{
							spot = rand() % 8;
							if (!LEnemy[spot].alive)
							{
								LEnemy[spot].alive = true;
								LEnemy[spot].xHealth = 100;
								++NumOfEnemies;
							}
						}

						for (int i = 0; i < TOTAL_ENEMY_BUTTONS && NumOfEnemiesReady != NumOfEnemies; ++i)
						{
							if (LEnemy[i].alive && LEnemy[i].a < 255)
							{
								LEnemy[i].Appearing(speed);
								LEnemy[i].setAlpha();
							}
							if (LEnemy[i].a == 255)
							{
								++NumOfEnemiesReady;
							}
						}
						
						Startime = SDL_GetTicks();
					}
					else if(shot)
					{
						for (shoti = 0; shoti < TOTAL_ENEMY_BUTTONS; ++shoti)
						{
							if (LEnemy[shoti].shot && LEnemy[shoti].alive && LEnemy[shoti].a > 0 && !LEnemy[shoti].xHealth)
							{
								LEnemy[shoti].DisAppearing(speed);
								LEnemy[shoti].setAlpha();
							}
							else if (LEnemy[shoti].shot && LEnemy[shoti].alive && !LEnemy[shoti].a && !LEnemy[shoti].xHealth)
							{
								--NumOfEnemies;
								--NumOfEnemiesReady;
								LEnemy[shoti].shot = LEnemy[shoti].alive = false;
								++kills;
								talalt = false;

								if (!(kills % 7) && kills != 0)
								{
									if (speed < 255)
										speed += 15;
									if (timeGap > 400)
										timeGap -= 300;

								}
								if (!(kills % 25) && kills != 0)
								{
									++TotalNumOfEnemies;
								}
							}
						}
					}
					if (SDL_GetTicks() - Startime > timeGap)
					{
						Startime = SDL_GetTicks();
						if (lifes > 0);
						--lifes;
						Startime = SDL_GetTicks();
					}
					//loading kills the user has got
					PointText.str(""); //initializing it empty
					PointText << "Kills: " << kills;
					if (!LPointTexture.loadFromRenderedText(PointText.str(), { 57, 255, 20 }))
					{
						logerror(std::cout, "loadFromRenderedText", E_TTF);
					}

					SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
					SDL_RenderClear(ren);

					LTetris.render(0, 0);

					//loading the remaining lifes					
					if (lifes >= 3)
					{
						LHeart.setPosition(1820, 0);
						LHeart.render(LHeart.xPosition.x, LHeart.xPosition.y);
					}
					else
					{
						LNoHeart.setPosition(1820, 0);
						LNoHeart.render(LNoHeart.xPosition.x, LNoHeart.xPosition.y);
					}
					if (lifes >= 2)
					{
						LHeart.setPosition(1710, 0);
						LHeart.render(LHeart.xPosition.x, LHeart.xPosition.y);
					}
					else
					{
						LNoHeart.setPosition(1710, 0);
						LNoHeart.render(LNoHeart.xPosition.x, LNoHeart.xPosition.y);
					}
					if (lifes >= 1)
					{
						LHeart.setPosition(1600, 0);
						LHeart.render(LHeart.xPosition.x, LHeart.xPosition.y);
					}
					else
					{
						LNoHeart.setPosition(1600, 0);
						LNoHeart.render(LNoHeart.xPosition.x, LNoHeart.xPosition.y);
					}

					for(int k = 0; k < TOTAL_ENEMY_BUTTONS; ++k)
						if(LEnemy[k].alive)
							LEnemy[k].render(LEnemy[k].xPosition.x, LEnemy[k].xPosition.y, LEnemy[k].xWidth, LEnemy[k].xHeight, &LEnemy[k].xFraction, LEnemy[k].xAngle, &LEnemy[k].xCenter, SDL_FLIP_HORIZONTAL);

					LPointTexture.render(SCREEN_WIDTH - LPointTexture.getWidth(), SCREEN_HEIGHT - LPointTexture.getHeight() - 100);

					if (Mix_Playing(-1) && voicechat)
						LToxic1.render(LToxic1.xPosition.x, LToxic1.xPosition.y);
				}
				else
				{
					LNoHeart.setPosition(1600, 0);
					LNoHeart.render(LNoHeart.xPosition.x, LNoHeart.xPosition.y);
					LDeath.setPosition(0, 0);
					LDeath.render(LDeath.xPosition.x, LDeath.xPosition.y, SCREEN_WIDTH, SCREEN_HEIGHT);
				}
				SDL_RenderPresent(ren);
				SDL_Delay(50);
			
			
			}
		}
	}

	close();
}