#include <vector>
#include <iostream>
#include <ctime>
#include <map>

typedef uint8_t uint8;
typedef uint32_t uint32;

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

uint8 MAX_COLUMNS 	= 8;
uint8 MAX_ROWS		= 8;
uint8 MAX_MINAS 	= 10;

enum Difficulty
{
	GAME_DIFFICULTY_EASY,
	GAME_DIFFICULTY_MEDIUM,
	GAME_DIFFICULTY_HARD
};

std::vector<std::vector<uint32>>  sBoard; // Tablero del juego
typedef std::map<Point*, bool> AvailablePoints;
AvailablePoints sAvailablePointsStore; // Contenedor usado para sacar puntos randoms
std::vector<Point*> sMinesStore; // Posiciones de las minas.
AvailablePoints sPointsAlreadyKnown;
std::vector<Point*> sPointContainer;
Point* playerMove = new Point(-1, -1);

void SetRowsColumnsMinesByDifficulty(uint32 difficulty)
{
	switch(difficulty)
	{
		case GAME_DIFFICULTY_EASY:
			MAX_COLUMNS = 8;
			MAX_ROWS = 8;
			MAX_MINAS = 10;
			break;

		case GAME_DIFFICULTY_MEDIUM:
			MAX_COLUMNS = 16;
			MAX_ROWS = 16;
			MAX_MINAS = 40;
			break;

		case GAME_DIFFICULTY_HARD:
			MAX_COLUMNS = 30;
			MAX_ROWS = 16;
			MAX_MINAS = 99;
			break;

		default:
			break;
	}
	sBoard.resize(MAX_ROWS, std::vector<uint32>(MAX_COLUMNS));
}

inline void InitializeRandom() { std::srand(std::time(nullptr)); }

Point* GetPoint(int r, int c)
{
	for(std::vector<Point*>::iterator itr = sPointContainer.begin(); itr != sPointContainer.end(); ++itr)
		if((*itr)->x == r && (*itr)->y == c)
			return (*itr);
	return NULL;
}

void InitializeAvailablePointsContainer()
{
	uint32 r = playerMove->x;
	uint32 c = playerMove->y;

	for(uint32 i = 0; i < MAX_ROWS; ++i)
		for(uint32 j = 0; j < MAX_COLUMNS; ++j)
		{
			Point* point = new Point(i, j);
			sAvailablePointsStore[point] = true; // Punto disponible
			sPointContainer.push_back(point);
		}
	
	// Filtramos al rededores del punto del player
	for(int i = r - 1; i < r + 2; ++i)
		for(int j = c - 1; j < c + 2; ++j)
			if(i >= 0 && j >= 0 && i < MAX_ROWS && j < MAX_COLUMNS)
				sAvailablePointsStore.erase(GetPoint(i, j));
}

void ShowAvailablePoints()
{
	for(AvailablePoints::iterator itr = sAvailablePointsStore.begin(); itr != sAvailablePointsStore.end(); ++itr)
		itr->first->ToString();
}

void InitializeMinesPositions()
{
	for(uint8 i = 0; i < MAX_MINAS; ++i)
	{
		AvailablePoints::iterator itr = sAvailablePointsStore.begin();
		std::advance(itr, std::rand() % sAvailablePointsStore.size());
		sAvailablePointsStore.erase(itr->first);

		sMinesStore.push_back(itr->first);
		sBoard[itr->first->x][itr->first->y]	= '*';
	}
}

void Discover(int r, int c)
{
	Point* point = GetPoint(r, c);
	if(sPointsAlreadyKnown[point]) return; // Punto ya explorado no se chequea.
	sPointsAlreadyKnown[point] = true;
	for(int i = r - 1; i < r + 2; ++i)
		for(int j = c - 1; j < c + 2; ++j)
			if(i >= 0 && j >= 0 && i < MAX_ROWS && j < MAX_COLUMNS)
				if(sBoard[r][c] == 0)
						Discover(i, j);			
}

void ShowBoard()
{
	std::cout << std::endl;
	for(uint8 i = 0; i < MAX_ROWS; ++i)
	{
		for(uint8 j = 0; j < MAX_COLUMNS; ++j)
		{
			if(sPointsAlreadyKnown[GetPoint(i, j)] == false || sBoard[i][j] == '*')
			{
				std::cout << "- ";
				continue;
			}
			std::cout << sBoard[i][j] << ' ';
		}
		std::cout << std::endl;		
	}
}

void CalcMinas()
{
	for(Point* point : sMinesStore)
	{
		int x = point->x;
		int y = point->y;
		for(int i = x - 1; i < x + 2; ++i)
			for(int j = y - 1; j < y + 2; ++j)
				if(i >= 0 && j >= 0 && i < MAX_ROWS && j < MAX_COLUMNS)
					if(sBoard[i][j] != '*')
						sBoard[i][j]++;		
	}
}

enum InputMode
{
	INPUT_MODE_DIFFICULTY,
	INPUT_MODE_MOVE
};

bool IsValidInput(uint32 mode, std::vector<uint32> inputContainer)
{
	switch(mode)
	{
		case INPUT_MODE_DIFFICULTY:
		{
			uint32 difficulty = inputContainer[0];
			if(difficulty < GAME_DIFFICULTY_EASY || difficulty > GAME_DIFFICULTY_HARD)
				return false;
			break;
		}

		case INPUT_MODE_MOVE:
		{
			uint32 x = inputContainer[0];
			uint32 y = inputContainer[1];
			if(x < 0 || y < 0 || x >= MAX_ROWS || y >= MAX_COLUMNS)
				return false;
			break;
		}

		default:
			return false;
	}
	return true;
}

void AskUserForDifficulty()
{
	std::cout << "Seleccionar dificultad: 0 (Facil) - 1 (Medio) - 2 (Dificil) ";

	uint32 diff; std::cin >> diff;

	if(!IsValidInput(INPUT_MODE_DIFFICULTY, { diff }))
	{
		system("cls");
		AskUserForDifficulty();
		return;
	}
	SetRowsColumnsMinesByDifficulty(diff);
}

void AskUserForMove()
{
	ShowBoard();
	std::cout << "Realiza un movimiento: ";
	uint32 x, y; std::cin >> x >> y;

	if(!IsValidInput(INPUT_MODE_MOVE, {x, y}))
	{
		system("cls");
		AskUserForMove();
		return;
	}
	playerMove->x = x;
	playerMove->y = y;
}

// Deberia ser inline ?
inline void RegisterMoveOnBoard() { Discover(playerMove->x, playerMove->y); }

 int main()
{
	InitializeRandom();
	AskUserForDifficulty();
	AskUserForMove();
	InitializeAvailablePointsContainer();
	InitializeMinesPositions();
	CalcMinas();
	RegisterMoveOnBoard();
	ShowBoard();

  	return 0;
}
