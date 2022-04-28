#include "Defines.h"

struct Point
{
	uint32 x;	// Coordenada x
	uint32 y; 	// Coordenada y

	// Simbolo en el tablero -> SOLO CAMBIA CUANDO: mina cuando pierde (isMine), un numero cuando lo descubre (soporte hasKnown y closeMines)
	char symbol;

	// Cuando no es una mina, es un numero
	uint32 closeMines;
	bool hasKnown;

	// Es mina
	bool isMine;

	Point(uint32 i, uint32 j) : x(i), y(j), symbol('-'), closeMines(0), hasKnown(false), isMine(false) { }

	void ToString()
	{
		std::cout << "Fila: " << x << std::endl
			<< "Columna: " << y << std::endl;
	}
};

uint8 MAX_COLUMNS = 8;
uint8 MAX_ROWS = 8;
uint8 MAX_MINAS = 10;

enum class GameDifficulty : uint8
{
	Easy,
	Medium,
	Hard
};

class Board
{
	std::vector<std::vector<Point*>> m_board;
	std::vector<Point*> m_mines;

	Board(uint32 rows, uint32 columns);
	void AddPoint(Point* point);
	void CalcNearMines();
	void GetPoint(uint32 r, uint32 c);
	void ShowBoard();
};
