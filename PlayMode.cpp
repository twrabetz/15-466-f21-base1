#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include "PPU466.hpp"
#include "LoadBinary.hpp"

#include <iostream>

#include <random>

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.
	
	//I only use 2 colors for each sprite
	std::array< uint8_t, 8 > standardBit0 = {
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
	};

	std::vector<std::array<uint8_t, 8>> sprites;
	loadBinary(sprites);

	srand((unsigned int)time(NULL));

	for (int i = 0; i < numFans; i++)
	{
		fans.emplace_back(Fan(fmod((float)rand(),256.0f), fmod((float)rand(),256.0f)));
		fanCollisionOffsets.emplace_back(glm::vec2(0.0f));
		if (i >= numFans - 3)
			fans[i].team = 2;
	}

	int xWallChoices[] = { 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };

	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			ppu.background[i + PPU466::BackgroundWidth * j] = 0;
		}
	}

	for (int i = 0; i < numWalls; i++)
	{
		bool done = false;
		while (!done)
		{
			int xIndex = (int)((float)rand() / RAND_MAX * 9.99f);
			int x = xWallChoices[xIndex];
			int y = ((int)((float)rand() / RAND_MAX * 29.999f) + 1) * 8;
			done = true;
			for (int j = 0; j < walls.size(); j++)
			{
				if (walls[j].x == x && walls[j].y == y)
				{
					done = false;
					break;
				}
			}
			if (done)
			{
				walls.emplace_back(Wall(x, y));
				ppu.background[(x/8) + PPU466::BackgroundWidth * (y/8)] = (1 << 8);
			}
		}
	}

	//Floor tiles
	ppu.tile_table[0].bit0 = standardBit0;
	ppu.tile_table[0].bit1 = sprites[2];

	//use sprite 31 as a "fan":
	ppu.tile_table[31].bit0 = standardBit0;
	ppu.tile_table[31].bit1 = sprites[1];

	//use sprite 32 as a "player":
	ppu.tile_table[32].bit0 = standardBit0;
	ppu.tile_table[32].bit1 = sprites[0];

	//background tiles:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x66, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x44, 0xff),
	};

	//Walls:
	ppu.palette_table[1] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0xee, 0xee, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0xff, 0xff, 0xff),
	};

	//used for the player:
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0xff, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//Enemy fans:
	ppu.palette_table[5] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xff, 0x40, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//friendly fans:
	ppu.palette_table[4] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xb5, 0xb5, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//black and grey basic fans
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xC3, 0xC3, 0xC3, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	constexpr float PlayerSpeed = 80.0f;
	if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
	if (right.pressed) player_at.x += PlayerSpeed * elapsed;
	if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	if (up.pressed) player_at.y += PlayerSpeed * elapsed;

	player_at = glm::vec2(fmod(player_at.x, 256.0f), fmod(player_at.y, 256.0f));
	player_at = glm::vec2(player_at.x < 0.0f ? player_at.x + 256.0f : player_at.x, player_at.y < 0.0f ? player_at.y + 256.0f : player_at.y);

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	//Move the ol fanz around
	for (int i = 0; i < fans.size(); i++)
	{
		fans[i].timeSinceChange += elapsed;
		if (fans[i].timeSinceChange >= fans[i].changeTime)
		{
			fans[i].timeSinceChange = 0;
			float randomX = ((float)rand() / RAND_MAX);
			float randomY = ((float)rand() / RAND_MAX);
			fans[i].direction = glm::vec2(randomX < 0.33f ? 1 : randomX < 0.66f ? -1 : 0, randomY < 0.33f ? 1 : randomY < 0.66f ? -1 : 0);
		}
		fans[i].position += fans[i].direction * elapsed * (fans[i].team == 0 ? fanSpeed : fanSpeed * 1.5f);
		fans[i].position = glm::vec2(fmod(fans[i].position.x, 256.0f), fmod(fans[i].position.y, 256.0f));
		fans[i].position = glm::vec2(fans[i].position.x < 0.0f ? fans[i].position.x + 256.0f : fans[i].position.x,
			fans[i].position.y < 0.0f ? fans[i].position.y + 256.0f : fans[i].position.y);
		if (fans[i].currentTeamTime > 0.0f)
		{
			fans[i].currentTeamTime -= elapsed;
			if (fans[i].currentTeamTime <= 0.0f)
				fans[i].team = 0;
		}
	}

	auto checkCollision = [](glm::vec2 pos1, glm::vec2 pos2) -> bool
	{
		float xDiff = abs(pos1.x - pos2.x);
		float yDiff = abs(pos1.y - pos2.y);
		return xDiff <= 8 && yDiff <= 8;
	};

	//Make everything go ahead n bump everything else
	auto processCollision = [](glm::vec2 pos1, glm::vec2 pos2, glm::vec2& offset, bool wall)
	{
		float xDiff = abs(pos1.x - pos2.x);
		float yDiff = abs(pos1.y - pos2.y);
		float xPush = std::max<float>(8.0f - abs(xDiff), 0);
		float yPush = std::max<float>(8.0f - abs(yDiff), 0);
		if( xDiff > yDiff )
			offset += glm::vec2(pos1.x >= pos2.x ? (wall ? xPush : xPush / 2.0f) : (wall ? -xPush : -xPush / 2.0f), 0.0f);
		else
			offset += glm::vec2(0.0f, pos1.y >= pos2.y ? (wall ? yPush : yPush / 2.0f) : (wall ? -yPush : -yPush / 2.0f));
	};

	playerCollisionOffset = glm::vec2(0.0f);
	for (int i = 0; i < numFans; i++)
	{
		fanCollisionOffsets[i] = glm::vec2(0.0f);
		assert(fans[i].position.x <= 256.0f && fans[i].position.y <= 256.0f);
	}

	for (int i = 0; i < numFans; i++)
	{
		if (checkCollision(player_at, fans[i].position))
		{
			if (fans[i].team == 0 || fans[i].team == 2)
			{
				fans[i].team = 1;
				fans[i].currentTeamTime = standardTeamTime;
			}
			processCollision(player_at, fans[i].position, playerCollisionOffset, false);
		}
	}
	for (int i = 0; i < numWalls; i++)
	{
		if (checkCollision(player_at, glm::vec2((float)walls[i].x, (float)walls[i].y)))
		{
			processCollision(player_at, glm::vec2((float)walls[i].x, (float)walls[i].y), playerCollisionOffset, true);
		}
	}

	for (int i = 0; i < numFans; i++)
	{
		if( checkCollision(player_at, fans[i].position) )
			processCollision(fans[i].position, player_at, fanCollisionOffsets[i], false);
		for (int j = 0; j < numFans; j++)
		{
			if (j != i && checkCollision(fans[i].position, fans[j].position))
			{
				if (fans[j].team == 0 && fans[i].team != 0)
				{
					fans[j].team = fans[i].team;
					fans[i].currentTeamTime = standardTeamTime;
				}
				processCollision(fans[i].position, fans[j].position, fanCollisionOffsets[i], false);
			}
		}
		for (int j = 0; j < numWalls; j++)
		{
			if (checkCollision(fans[i].position, glm::vec2((float)walls[j].x, (float)walls[j].y)))
			{
				processCollision(fans[i].position, glm::vec2((float)walls[j].x, (float)walls[j].y), fanCollisionOffsets[i], true);
			}
		}
	}

	player_at += playerCollisionOffset;
	for (int i = 0; i < numFans; i++)
	{
		fans[i].position += fanCollisionOffsets[i];
	}

	player_at = glm::vec2(fmod(player_at.x, 256.0f), fmod(player_at.y, 256.0f));
	player_at = glm::vec2(player_at.x < 0.0f ? player_at.x + 256.0f : player_at.x, player_at.y < 0.0f ? player_at.y + 256.0f : player_at.y);
	for (int i = 0; i < numFans; i++)
	{
		fans[i].position = glm::vec2(fmod(fans[i].position.x, 256.0f), fmod(fans[i].position.y, 256.0f));
		fans[i].position = glm::vec2(fans[i].position.x < 0.0f ? fans[i].position.x + 256.0f : fans[i].position.x,
			fans[i].position.y < 0.0f ? fans[i].position.y + 256.0f : fans[i].position.y);
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background scroll:
	//ppu.background_position.x = int32_t(-0.5f * player_at.x);
	//ppu.background_position.y = int32_t(-0.5f * player_at.y);

	//player sprite:
	ppu.sprites[0].x = int32_t(player_at.x);
	ppu.sprites[0].y = int32_t(player_at.y);
	ppu.sprites[0].index = 32;
	ppu.sprites[0].attributes = 7;

	//some other misc sprites:
	for (int32_t i = 1; i < numFans; ++i) {
		//ppu.sprites[i].x = int32_t(0.5f * PPU466::ScreenWidth + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * PPU466::ScreenWidth);
		//ppu.sprites[i].y = int32_t(0.5f * PPU466::ScreenHeight + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * PPU466::ScreenWidth);
		ppu.sprites[i].x = (int)fans[i].position.x;
		ppu.sprites[i].y = (int)fans[i].position.y;
		ppu.sprites[i].index = 31;
		ppu.sprites[i].attributes = fans[i].team == 0 ? 6 : (fans[i].team == 1 ? 4 : 5);
	}

	//--- actually draw ---
	ppu.draw(drawable_size);
}
