#include "Defines.h"

struct Point
{
	uint32 x;	// Coordenada x
	uint32 y; 	// Coordenada y
	Point(uint32 i, uint32 j) : x(i), y(j) { }
	void ToString()
	{
		std::cout << "Fila: " << x << std::endl
			<< "Columna: " << y << std::endl;
	}
};

uint8 MAX_COLUMNS = 8;
uint8 MAX_ROWS = 8;
uint8 MAX_MINAS = 10;

enum Difficulty
{
	GAME_DIFFICULTY_EASY,
	GAME_DIFFICULTY_MEDIUM,
	GAME_DIFFICULTY_HARD
};