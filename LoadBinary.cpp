#include "LoadBinary.hpp"
#include "read_write_chunk.hpp"
#include "data_path.hpp"
#include <fstream>
#include <iostream>

void loadBinary( std::vector<std::array<uint8_t,8>>& result )
{
	std::string pathsToLoad[3];

	std::string directory = data_path("../binaries");
	pathsToLoad[0] = directory + "/Rockstar.sprt";
	pathsToLoad[1] = directory + "/Fan.sprt";
	pathsToLoad[2] = directory + "/Floor.sprt";

	for (int i = 0; i < 3; i++)
	{
		
		std::cout << pathsToLoad[i] << std::endl;

		std::ifstream stream(pathsToLoad[i], std::ifstream::binary | std::ifstream::in);

		assert(stream.is_open());

		std::vector<uint8_t> lines;

		read_chunk<uint8_t>(stream, "SPRT", &lines);

		std::array< uint8_t, 8 > bit0;
		for (int j = 0; j < 8; j++)
		{
			bit0[j] = lines[j];
		}

		result.push_back(bit0);

		stream.close();

	}
}