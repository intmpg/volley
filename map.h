#include <SFML/Graphics.hpp>
#include <iostream>
const int HEIGHT_MAP = 25;
const int WIDTH_MAP = 41;

using namespace sf;

sf::String TileMap[HEIGHT_MAP] = {
	"0000000000000000000000000000000000000000",
	"00                                     0",
	"00                                     0",
	"00                                     0",
	"00                                     0",
	"00                                     0",
	"00                                     0",
	"00                                     0",
	"00                                     0",
	"00                                     0",
	"00                                     0",
	"00                  0                  0",
	"00                  0                  0",
	"00                  0                  0",
	"00                  0                  0",
	"00                  0                  0",
	"00                  0                  0",
	"00                  0                  0",
	"00                  0                  0",
	"00                  0                  0",
	"00                  0                  0",
	"0000000000000000000000000000000000000000",
	"0000000000000000000000000000000000000000",
	"0000000000000000000000000000000000000000",
	"0000000000000000000000000000000000000000"
};

void drawMap(sf::Sprite &s_map, sf::RenderWindow &window){
	for (int yi = 0; yi < HEIGHT_MAP; yi++)
	for (int xj = 0; xj < WIDTH_MAP; xj++)
	{
		if (TileMap[yi][xj] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
		if (TileMap[yi][xj] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
		if ((TileMap[yi][xj] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));
		if ((TileMap[yi][xj] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
		if ((TileMap[yi][xj] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
		s_map.setPosition(xj * 32, yi * 32);

		window.draw(s_map);
	}
}