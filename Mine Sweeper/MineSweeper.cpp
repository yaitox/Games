#include <vector>
#include <ctime>
#include "Board.h"

std::vector<std::vector<Point*>>  sBoard; // Tablero del juego
std::vector<Point*> sAvailablePointsStore; // Contenedor usado para sacar puntos randoms
std::vector<Point*> sMinesStore; // Posiciones de las minas.
Point* playerMove = nullptr;

void SetRowsColumnsMinesByDifficulty(GameDifficulty difficulty)
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
	sBoard.resize(MAX_ROWS, std::vector<Point*>(MAX_COLUMNS));
}

inline void InitializeRandom() { std::srand((unsigned int)std::time(nullptr)); }

enum InputMode
{
	INPUT_MODE_DIFFICULTY,
	INPUT_MODE_MOVE
};

bool IsValidInput(uint32 mode, std::vector<int> inputContainer)
{
	switch (mode)
	{
		case INPUT_MODE_DIFFICULTY:
		{
			int input = inputContainer[0];
			if (input >= static_cast<int>(GameDifficulty::Easy) && input <= static_cast<int>(GameDifficulty::Hard))
				return true;
		}

		case INPUT_MODE_MOVE:
		{
			int x = inputContainer[0];
			int y = inputContainer[1];
			
			if (x >= 0 && y >= 0 && x < MAX_ROWS && y < MAX_COLUMNS)
				return true;
		}

		default:
			break;
	}
	return false;
}

Point* GetPoint(int r, int c) 
{ 
	if (IsValidInput(INPUT_MODE_MOVE, { r, c }))
		return sBoard[r][c];
	return nullptr;
}

void InitializeAvailablePointsContainer()
{
	for (uint32 i = 0; i < MAX_ROWS; ++i)
		for (uint32 j = 0; j < MAX_COLUMNS; ++j)
		{
			Point* point = new Point(i, j);
			sAvailablePointsStore.push_back(point); // Punto disponible
			sBoard[i][j] = point;
		}
}

void ShowAvailablePoints()
{
	for (std::vector<Point*>::iterator itr = sAvailablePointsStore.begin(); itr != sAvailablePointsStore.end(); ++itr)
		(*itr)->ToString();
}

void CalcMinas()
{
	for (Point* point : sMinesStore)
	{
		int x = (int)point->x;
		int y = (int)point->y;

		for (int i = x - 1; i < x + 2; ++i)
			for (int j = y - 1; j < y + 2; ++j)
				if (Point* closePoint = GetPoint(i, j))
					if (!closePoint->isMine)
						closePoint->closeMines++;
	}
}

void InitializeMinesPositions()
{
	for (uint8 i = 0; i < MAX_MINAS; ++i)
	{
		std::vector<Point*>::iterator itr = sAvailablePointsStore.begin();
		std::advance(itr, std::rand() % sAvailablePointsStore.size());

		Point* mine = *itr;
		mine->isMine = true;
		sMinesStore.push_back(mine);
		sAvailablePointsStore.erase(itr);
	}
	CalcMinas();
}

void Discover(int r, int c)
{
	Point* point = GetPoint(r, c);
	if (point->hasKnown || point->isMine) return; // Punto ya explorado no se chequea.
	point->hasKnown = true;

	for (int i = r - 1; i < r + 2; ++i)
		for (int j = c - 1; j < c + 2; ++j)
			if (IsValidInput(INPUT_MODE_MOVE, { i, j }))
				if (point->closeMines == 0)
					Discover(i, j);				
}

void ShowBoard()
{
	std::cout << std::endl;
	for (uint32 i = 0; i < MAX_ROWS; ++i)
	{
		for (uint32 j = 0; j < MAX_COLUMNS; ++j)
		{
			Point* point = GetPoint(i, j);
			if (!playerMove) // No hay movimiento del jugador aun
				std::cout << point->symbol;
			else
			{
				if (!playerMove->isMine)
				{
					if (point->hasKnown)
						std::cout << point->closeMines;
					else
						std::cout << point->symbol;
				}
				else
				{
					if (point->isMine)
						std::cout << '*';
					else
						std::cout << point->closeMines;
				}
			}
			std::cout << ' ';
		}
		std::cout << std::endl;
	}
}

void AskUserForDifficulty()
{
	std::cout << "Seleccionar dificultad: 0 (Facil) - 1 (Medio) - 2 (Dificil) ";

	uint32 diff; std::cin >> diff;

	if (!IsValidInput(INPUT_MODE_DIFFICULTY, { (int)diff }))
	{
		system("cls");
		AskUserForDifficulty();
		return;
	}
	SetRowsColumnsMinesByDifficulty(static_cast<GameDifficulty>(diff));
}

void Initialize()
{
	InitializeAvailablePointsContainer();
	InitializeMinesPositions();
}

// Deberia ser inline ?
inline void RegisterMoveOnBoard() { Discover(playerMove->x, playerMove->y); }

// TODO: implementar poder meter banderas en las casillas
void AskUserForMove()
{
	ShowBoard();
	std::cout << "Realiza un movimiento: ";
	uint32 x, y; std::cin >> x >> y;

	Point* point = GetPoint(x, y);

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
		ShowBoard();
		std::cout << "HAS PERDIDO!" << std::endl;
		system("pause");
		return;
	}
	AskUserForMove();
	RegisterMoveOnBoard();
	ShowBoard();
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
