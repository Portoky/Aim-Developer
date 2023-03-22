#pragma once
#include <iostream>
#include "SDL.h"
#include <string>
#include <sstream>
#undef main

std::string getResourcePath(const std::string& subDir = "") {
	//We need to choose the path separator properly based on which
	//platform we're running on, since Windows uses a different
	//separator than most systems
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif
	//This will hold the base resource path: Lessons/res/
	//We give it static lifetime so that we'll only need to call
	//SDL_GetBasePath once to get the executable path
	static std::string baseRes;
	if (baseRes.empty()) {
		//SDL_GetBasePath will return NULL if something went wrong in retrieving the path
		char* basePath = SDL_GetBasePath(); //returns the path of the executable file
		if (basePath) {
			baseRes = basePath;
			SDL_free(basePath);
		}
		else {
			std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
			return "";
		}
		//We replace the last bin/ with res/ to get the the resource path
		std::stringstream ss;
		//ss << "x64";
		//ss << PATH_SEP;
		ss << "Debug";
		std::string xd;
		xd = ss.str();
		//std::cout << xd << "\n";
		size_t pos = baseRes.rfind(xd);
		baseRes = baseRes.substr(0, pos);
	}
	//If we want a specific subdirectory path in the resource directory
	//append it to the base path. This would be something like Lessons/res/Lesson0
	return subDir.empty() ? baseRes : baseRes + subDir;
}
