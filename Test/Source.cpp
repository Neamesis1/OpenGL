#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <filesystem>
#include <Windows.h>


std::string return_base_path(std::string user_string)
{
	for (int i = user_string.length(); i > -1; i--)
	{
		if (user_string[i] == '/') { break; }

		user_string.erase(user_string.begin() + i);
	}

	user_string.erase(user_string.length() - 1);

	for (int i = user_string.length(); i > -1; i--)
	{
		if (user_string[i] == '/') { break; }

		user_string.erase(user_string.begin() + i);
	}

	return user_string;
}


std::string return_exe_path()
{
	char exe_path[MAX_PATH];
	GetModuleFileNameA(NULL, exe_path, sizeof(exe_path));

	std::string executable = exe_path;

	for (int i = 0; i < (int)executable.length(); i++)
	{
		if (executable[i] == '\\')
		{
			executable[i] = '/';
		}
	}

	return executable;
}


int main(int argc, char** argv)
{

	std::string path = return_base_path(return_exe_path()) + "OpenGL/res/wallpaper_milkyway.jpg";

	const char* file_path = path.c_str();

	//std::cout << std::filesystem::current_path().generic_string() << '\n';

	/*assert(argc > 1);
	char const* image_path = argv[1];*/

	int width, height, nrChannels;
	unsigned char* data = stbi_load(file_path, &width, &height, &nrChannels, 4);
	
	if (data)
	{
		std::cout << "failed to load file: " << stbi_failure_reason() << '\n';
	}
	else
	{
		std::cout << "image loaded successfully" << '\n';
	}

}
