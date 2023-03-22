#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

const int TOTAL_ENEMY_BUTTONS = 8;

enum errors : Uint8
{
	E_SDL, E_IMG, E_TTF, E_MIX
};

void logerror(std::ostream& os, std::string mess, errors e)
{
	if (e == E_SDL)
		os << mess << " ERROR: " << SDL_GetError();
	else if (e == E_IMG)
		os << mess << " ERROR: " << IMG_GetError();
	else if (e == E_MIX)
		os << mess << " ERROR: " << Mix_GetError();
	else if (e == E_TTF)
		os << mess << " ERROR: " << TTF_GetError();
}

SDL_Renderer* ren = NULL;
TTF_Font* gFont = NULL;

///***///CLASSES
class LTexture
{
public:
	LTexture();
	~LTexture();
	bool loadFromFile(std::string const path);
	void free();
	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	void setBlendMode(SDL_BlendMode blending);
	void setAlpha();
	void render(int x = 0, int y = 0, int xWidth = 0, int xHeight = 0, SDL_Rect* clip = NULL, double angle = 0.0,
		SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
	void setPosition(int x, int y); //hol legyen a kep bal felso sarka
	void setMeasure(int Width, int Height); //mekkora legyen a kep
	void setAngle(double angle); //doles szoget allitja be
	void setFraction(SDL_Rect body); //mennyi kell a kepbol, x, y (bal felsso sarok), s abbol mennyit vagjon le: width, height
	void setCenter(SDL_Point middle); //hol legyen a pont ami korul forgatjuk a kepet
	bool handleEvent(SDL_Event* e);	//megnezi, hogyha volt loves, vki lelottek-e, ha missclick, eletlevonas lesz
	void Appearing(int speed); //igy jelenik meg az ellenseg gyorsasag
	void DisAppearing(int speed);
	int getWidth();
	int getHeight();

	int xWidth; // mennyit akarunk belole
	int xHeight;
	SDL_Point xPosition;// a kep bal felso sarkanak a koordinatai
	SDL_Point xCenter;	//a kep pontja, ami korul forgatjuk azt
	double xAngle;	//dolesszoge a kepnek
	SDL_Rect xFraction;	//mekkora legyen a kep
	bool alive, shot;
	int a;
	int xHealth;

private:
	SDL_Texture* mTexture;
	int mWidth;//mekkora a kep
	int mHeight;


};


///***///classes function

//LTextures Functions
LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	xWidth = 0;
	xHeight = 0;
	xAngle = 0.0;
	xPosition.x = xPosition.y = 0;
	xCenter.x = xCenter.y = 0;
	xFraction.x = xFraction.y = 0;
	alive = false;
	shot = false;
	a = 0;
	xHealth = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile(std::string const path)
{
	free();

	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		logerror(std::cout, "IMG_Load", E_IMG);
		return false;
	}

	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 255, 255));
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 74, 255, 28));

	newTexture = SDL_CreateTextureFromSurface(ren, loadedSurface);
	if (newTexture == NULL)
	{
		logerror(std::cout, "SDL_CreateTexture", E_SDL);
		return false;
	}

	mWidth = loadedSurface->w;
	mHeight = loadedSurface->h;
	SDL_FreeSurface(loadedSurface);

	xCenter.x = mWidth / 2;
	xCenter.y = mHeight / 2;

	mTexture = newTexture;

	return true;
}

void LTexture::free()
{
	if (mTexture != NULL)
	{
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	free();
	SDL_Surface* textSurface = TTF_RenderUTF8_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface == NULL)
	{
		logerror(std::cout, "TTF_RenderUTF8_Solid()", E_TTF);
		return false;
	}

	mTexture = SDL_CreateTextureFromSurface(ren, textSurface);
	if (mTexture == NULL)
	{
		logerror(std::cout, "SDL_CreateTextureFromSurface()", E_SDL);
		return false;
	}

	mWidth = textSurface->w;
	mHeight = textSurface->h;
	SDL_FreeSurface(textSurface);

	return true;
}

void LTexture::render(int x, int y, int xWidth, int xHeight, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) //mekkora legyen a kep
{
	SDL_Rect renderQuad;
	if (xWidth == 0 || xHeight == 0) //mekkora legyen a kep
	{
		renderQuad = { x, y, mWidth, mHeight };
	}
	else
	{
		renderQuad = { x, y, xWidth, xHeight };
	}
	if (clip == NULL || clip->w == 0 || clip->h == 0)
	{
		SDL_RenderCopyEx(ren, mTexture, NULL, &renderQuad, angle, center, flip);
	}
	else
	{
		SDL_RenderCopyEx(ren, mTexture, clip, &renderQuad, angle, center, flip); //clip: mennyit akarunk a kepbol, renderquad: mekkora legyen a kep
	}
	
}

void LTexture::setPosition(int x, int y)
{
	xPosition.x = x;
	xPosition.y = y;
}

void LTexture::setAngle(double angle)
{
	xAngle = angle;
}

void LTexture::setMeasure(int Width, int Height) //mennyi
{
	xWidth = Width;
	xHeight = Height;
}

bool LTexture::handleEvent(SDL_Event* e)
{
	//if mouse event happened
	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		//get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);
		
		bool inside = true;

		if (x < xPosition.x)
		{
			inside = false;
		}
		else if (x > xPosition.x + xWidth)
		{
			inside = false;
		}
		else if (y < xPosition.y)
		{
			inside = false;
		}
		else if (y > xPosition.y + xHeight)
		{
			inside = false;
		}

		if (inside)
		{
			/*if (y >= (xPosition.y + 100))
			{
				xHealth -= 100;
			}
			else if (y < (xPosition.y + 100) && y >= xPosition.y + 400)
			{
				if (xHealth >= 50)
					xHealth -= 50;
				else
					xHealth = 0;
			}
			else if (y < (xPosition.y + 400))
			{
				if (xHealth >= 30)
					xHealth -= 30;
				else
					xHealth = 0;
			}*/
			xHealth -= 50;
		}
		return inside;

	}

	return false;
}


void LTexture::setFraction(SDL_Rect body)
{
	xFraction = body; //mennyi kell a kepbol
}

void LTexture::setCenter(SDL_Point middle)
{
	xCenter.x = middle.x;
	xCenter.y = middle.y;
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha()
{
	SDL_SetTextureAlphaMod(mTexture, a);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

void LTexture::Appearing(int speed)
{
	if (a + speed > 255)
	{
		a = 255;
	}
	else if (a + speed <= 255)
	{
		a += speed;
	}
}

void LTexture::DisAppearing(int speed)
{
	if (a - speed >= 0)
		a -= speed;
	else
		a = 0;
}