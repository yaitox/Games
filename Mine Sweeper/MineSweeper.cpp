#include <vector>
#include <iostream>
#include <ctime>
#include <set>
#include <map>

typedef uint8_t uint8;
typedef uint32_t uint32;

struct Point // TODO: buscar manera de eliminar adyacentes con este struct. 
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

std::vector<std::vector<char>>  sBoard; // Tablero del juego
typedef std::map<std::pair<uint8, uint8>, bool> AvailablePoints; // TODO: actualmente se usa tambien para la funcion Discover. Deberia crearse otro contenedor?
AvailablePoints sAvailablePointsStore; // Contenedor usado para sacar puntos randoms
std::vector<std::pair<uint8, uint8>> sMinesStore; // Posiciones de las minas.

void SetRowsColumnsMinesByDifficulty(uint8 difficulty)
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
	sBoard.resize(MAX_ROWS, std::vector<char>(MAX_COLUMNS));
}

inline void InitializeRandom() { std::srand(std::time(nullptr)); }

void InitializeAvailablePointsContainer(int r, int c)
{
	for(uint8 i = 0; i < MAX_ROWS; ++i)
		for(uint8 j = 0; j < MAX_COLUMNS; ++j)
			sAvailablePointsStore[{i, j}] = true; // Punto disponible

	// Filtramos al rededores del punto del player
	for(int i = r - 1; i < r + 2; ++i)
		for(int j = c - 1; j < c + 2; ++j)
			if(i >= 0 && j >= 0 && i < MAX_ROWS && j < MAX_COLUMNS)
				sAvailablePointsStore.erase({i, j}); // Borra tambien negativos, aunque no existen.
}

void ShowAvailablePoints()
{
	uint32 total = 0;
	for(AvailablePoints::iterator itr = sAvailablePointsStore.begin(); itr != sAvailablePointsStore.end(); ++itr)
		printf("Pareja %d: {%d, %d}\n", ++total, itr->first.first, itr->first.second);
}

void InitializeMinesPositions()
{
	for(uint8 i = 0; i < MAX_MINAS; ++i)
	{
		AvailablePoints::const_iterator itr = sAvailablePointsStore.begin();
		std::advance(itr, std::rand() % sAvailablePointsStore.size());
		sAvailablePointsStore.erase(itr->first);

		sMinesStore.push_back(itr->first);
		sBoard[itr->first.first][itr->first.second]	= '*';
	}
	sAvailablePointsStore.clear(); // Aprovechamos el mapa para el discover
}

void Discover(int r, int c)
{
	if(sAvailablePointsStore[{r, c}]) return; // Punto ya explorado no se chequea.
	sAvailablePointsStore[{r, c}] = true;
	for(int i = r - 1; i < r + 2; ++i)
		for(int j = c - 1; j < c + 2; ++j)
			if(i >= 0 && j >= 0 && i < MAX_ROWS && j < MAX_COLUMNS)
				if(sBoard[i][j] == 0)
					Discover(i, j);		
}

void ShowBoard()
{
	for(uint8 i = 0; i < MAX_ROWS; ++i)
	{
		for(uint8 j = 0; j < MAX_COLUMNS; ++j)
		{
			char c = sBoard[i][j];
			if(c != '*')
				std::cout << int(c) << ' ';
			else
				std::cout << c << ' ';
		}
		std::cout << std::endl;		
	}
}

/* 
arriba izquierda: -fila -col x - 1, y - 1
Arriba: -fila =columna x - 1, y
arriba derecha: -fila +col x - 1, y + 1

Izquierda: =fila -columna x, y - 1
Yo mismo x, y
Derecha: =fila +columna x, y + 1

abajo izquierda: +fila -col x + 1, y - 1
Abajo: +fila =columna x + 1, y
abajo derecha: +fila +col x + 1, y + 1
*/

void CalcMinas()
{
	for(std::pair<uint8, uint8> pos : sMinesStore)
	{
		int x = pos.first;
		int y = pos.second;
		for(int i = x - 1; i < x + 2; ++i)
			for(int j = y - 1; j < y + 2; ++j)
			{
				if(i >= 0 && j >= 0 && i < MAX_ROWS && j < MAX_COLUMNS)
					if(sBoard[i][j] != '*')
						sBoard[i][j]++;
			}
	}
}

 int main()
{
	SetRowsColumnsMinesByDifficulty(0);
	InitializeRandom();
	InitializeAvailablePointsContainer(5,5); // TODO: preguntar al player por punto inicial.
	InitializeMinesPositions();
	CalcMinas();
	ShowBoard();

  	return 0;
}
