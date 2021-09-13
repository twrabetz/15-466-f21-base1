#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct Fan
{
	glm::vec2 position;
	glm::vec2 direction;
	float timeSinceChange = 0;
	float changeTime;

	int team = 0;

	float currentTeamTime = 0.0f;

	Fan(float newX, float newY)
	{
		position = glm::vec2(newX, newY);
		changeTime = 0.5f + 1.0f * ((float)rand() / RAND_MAX);
		direction = glm::vec2(((float)rand() / RAND_MAX) < 0.5f ? 1 : -1, ((float)rand() / RAND_MAX) < 0.5f ? 1 : -1);
	}
};

struct Wall
{
	int x;
	int y;

	Wall(int newX, int newY)
	{
		x = newX;
		y = newY;
	}
};

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);

	//the fanz
	std::vector<Fan> fans;
	const int numFans = 63;
	const float fanSpeed = 30.0f;
	const float standardTeamTime = 2.0f;

	std::vector<Wall> walls;
	int numWalls = 35;

	//collision stuff
	std::vector<glm::vec2> fanCollisionOffsets;
	glm::vec2 playerCollisionOffset;

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
