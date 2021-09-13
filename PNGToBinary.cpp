#include "load_save_png.hpp"
#include "read_write_chunk.hpp"
#include "data_path.hpp"
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>

int main(int argc, char** argv) {

	std::string pathsToLoad[3];
	std::string outNames[] = { "/Rockstar.sprt", "/Fan.sprt", "/Floor.sprt" };

	std::string directory = data_path("../images");
	std::string outDirectory = data_path("../binaries");
	pathsToLoad[0] = directory + "/Rockstar.png";
	pathsToLoad[1] = directory + "/BasicFan.png";
	pathsToLoad[2] = directory + "/FloorTile1.png";

	std::cout << "Hello World!" << std::endl;

	for (int pathIdx = 0; pathIdx < 3; ++pathIdx)
	{
		std::cout << "For Iteration" << std::endl;
		std::ofstream out(outDirectory + outNames[pathIdx], std::ofstream::binary);
		std::cout << "Created Ofstream" << std::endl;
		std::vector<glm::u8vec4> data;
		load_png(pathsToLoad[pathIdx], &(glm::uvec2(8, 8)), &data, OriginLocation::LowerLeftOrigin);
		std::cout << "Loaded png" << std::endl;
		std::vector<uint8_t> lines;
		for (int j = 0; j < data.size(); j += 8)
		{
			uint8_t line = 0;
			for (int i = j; i < j + 8; ++i)
			{
				std::cout << i << std::endl;
				int bit = 0;
				if (data[i][0] == 0 && data[i][1] == 0 && data[i][2] == 0)
					bit = 1;
				//Note: My sprites all have horizontal symmetry
				line |= bit << (i - j);
			}
			lines.push_back(line);
		}
		write_chunk<uint8_t>("SPRT", lines, &out);
		out.close();
	}
	
	std::cout << "Returning 0";

	return 0;
}