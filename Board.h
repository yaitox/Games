#include "Defines.h"
#include <vector>

enum class GameDifficulty : uint8
{
	Easy,
	Medium,
	Hard,

	Max
};

enum InputMode
{
	INPUT_MODE_DIFFICULTY,
	INPUT_MODE_MOVE
};

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

	// Sistema de flags
	bool isFlag;

	Point(uint32 i, uint32 j) : x(i), y(j), symbol('-'), closeMines(0), hasKnown(false), isMine(false), isFlag(false) { }

	void ToString()
	{
		std::cout << "Punto posicionado en:" << std::endl
			<< "Fila: " << x << ' '
			<< "Columna: " << y << std::endl;

		if (isMine)
			std::cout << "Es mina" << std::endl;
		else
			std::cout << (hasKnown ? "Ha sido descubierto por el jugador" : "No ha sido descubierto por el jugador")
			<< " y tiene " << closeMines << " minas al rededor." << "\n\n";
	}
};

class Board
{
private:
	std::vector<std::vector<Point*>> m_board;
	std::vector<Point*> m_mines;
	uint32 m_rows;
	uint32 m_columns;
	GameDifficulty m_difficulty;

public:
	Board(GameDifficulty difficulty);
	void AddPoint(Point* point);
	void CalcNearPointsFromMine(Point* mine);
	Point* GetPoint(int row, int col);
	void ShowBoard();
	uint32 GetRows() { return m_rows; };
	uint32 GetColums() { return m_columns; }
	bool ContainsPoint(int row, int col);

	// void SetDifficulty(GameDifficulty difficulty) { this->m_difficulty = difficulty; }
	GameDifficulty GetDifficulty() { return m_difficulty; }
};
