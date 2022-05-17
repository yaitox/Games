#include <ctime>
#include "Board.h"

std::vector<Point*> sAvailablePointsStore; // Contenedor usado para sacar puntos randoms
std::vector<Point*> sMinesStore; // Posiciones de las minas.

Point* playerMove = nullptr;
Board* sBoard = nullptr;

uint8 MAX_COLUMNS = 8;
uint8 MAX_ROWS = 8;
uint8 MAX_MINAS = 10;

void SetBoardSizeByDifficulty(GameDifficulty difficulty)
{
	switch (difficulty)
	{
		case GameDifficulty::Easy:
			MAX_COLUMNS = 8;
			MAX_ROWS = 8;
			MAX_MINAS = 10;
			break;

		case GameDifficulty::Medium:
			MAX_COLUMNS = 16;
			MAX_ROWS = 16;
			MAX_MINAS = 40;
			break;

		case GameDifficulty::Hard:
			MAX_COLUMNS = 30;
			MAX_ROWS = 16;
			MAX_MINAS = 99;
			break;

		default:
			break;
	}
	sBoard = new Board(MAX_ROWS, MAX_COLUMNS);
}

inline void InitializeRandom() { std::srand((unsigned int)std::time(nullptr)); }

void InitializeAvailablePointsContainer()
{
	for (uint32 row = 0; row < MAX_ROWS; ++row)
		for (uint32 col = 0; col < MAX_COLUMNS; ++col)
		{

			Point* point = new Point(row, col);
			sBoard->AddPoint(point);
			sAvailablePointsStore.push_back(point); // Punto disponible
		}
}

void ShowAvailablePoints()
{
	for (std::vector<Point*>::iterator itr = sAvailablePointsStore.begin(); itr != sAvailablePointsStore.end(); ++itr)
		(*itr)->ToString();
}

void InitializeMinesPositions()
{
	for (uint8 i = 0; i < MAX_MINAS; ++i)
	{
		std::vector<Point*>::iterator itr = sAvailablePointsStore.begin();
		std::advance(itr, std::rand() % sAvailablePointsStore.size());

		Point* mine = *itr;
		mine->isMine = true;
		sBoard->CalcNearPointsFromMine(mine);

		sAvailablePointsStore.erase(itr);
	}	
}

void DiscoverPoint(Point* point)
{
	// Punto ya explorado o mina no se chequea.
	if (point->hasKnown || point->isMine) 
		return; 

	int pointRow = (int)point->x;
	int pointCol = (int)point->y;
	point->hasKnown = true;

	for (int row = pointRow - 1; row <= pointRow + 1; ++row)
		for (int col = pointCol - 1; col <= pointCol + 1; ++col)
			if (Point* nearPoint = sBoard->GetPoint(row, col))
				if (point->closeMines == 0) // Solo los 0 pueden descubrir varias casillas
					DiscoverPoint(nearPoint);
}

void Board::ShowBoard()
{
	std::cout << std::endl;
	for (uint32 i = 0; i < MAX_ROWS; ++i)
	{
		for (uint32 j = 0; j < MAX_COLUMNS; ++j)
		{
			Point* point = sBoard->GetPoint(i, j);

			if (playerMove && playerMove->isMine) // El player pierde, mostramos todo el tablero.
			{
				if (point->isMine)
					std::cout << '*';
				else
					std::cout << point->closeMines;
			}
			else
			{
				if (point->hasKnown)
					std::cout << point->closeMines;
				else
					std::cout << '-';
			}
			std::cout << ' ';
		}
		std::cout << std::endl;
	}
}

bool IsValidDifficulty(int difficulty)
{
	return (difficulty >= static_cast<uint8>(GameDifficulty::Easy) && difficulty <= static_cast<uint8>(GameDifficulty::Hard));
}

void AskUserForDifficulty()
{
	std::cout << "Seleccionar dificultad: 0 (Facil) - 1 (Medio) - 2 (Dificil) ";

	uint32 difficulty; std::cin >> difficulty;

	if (!IsValidDifficulty(difficulty))
	{
		system("cls");
		AskUserForDifficulty();
		return;
	}
	
	SetBoardSizeByDifficulty(static_cast<GameDifficulty>(difficulty));
}

void Initialize()
{
	InitializeAvailablePointsContainer();
	InitializeMinesPositions();
}

// TODO: implementar poder meter banderas en las casillas
void AskUserForMove()
{
	sBoard->ShowBoard();
	
	std::cout << "Realiza un movimiento: ";
	uint32 x, y; std::cin >> x >> y;

	Point* point = sBoard->GetPoint(x, y);

	if (!point)
	{
		system("cls");
		AskUserForMove();
		return;
	}
	playerMove = point;
}

// Siempre sigue esta logica: pedimos, registramos, mostramos.
void PlayGame()
{
	system("cls");
	
	if (playerMove->isMine)
	{
		sBoard->ShowBoard();
		std::cout << "HAS PERDIDO!" << std::endl;
		system("pause");
		return;
	}
	
	AskUserForMove();
	DiscoverPoint(playerMove);
	sBoard->ShowBoard();
	PlayGame();
}

int main()
{
	InitializeRandom();
	AskUserForDifficulty();
	Initialize();
	AskUserForMove();
	RegisterMoveOnBoard();
	PlayGame();

	return 0;
}
