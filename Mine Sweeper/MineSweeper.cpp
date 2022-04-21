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

// TODO: las filas, columnas y minas se calculan a partir de la dificultad escogida.
uint8 const MAX_COLUMNS = 30;
uint8 const MAX_ROWS	= 16;
uint8 const MAX_MINAS 	= 99;

typedef std::map<std::pair<uint32, uint32>, bool> AvailablePoints; // TODO: bool no se utiliza. Se usa map porque con vector por alguna razon no elimina bien el elemento del vector.
AvailablePoints sAvailablePointsStore; // Contenedor usado para sacar puntos randoms
std::vector<std::pair<uint32, uint32>> sMinesStore; // Posiciones de las minas.

inline void InitializeRandom() { std::srand(std::time(nullptr)); }

void InitializeAvailablePointsContainer(int r, int c)
{
	for(uint32 i = 0; i < MAX_ROWS; ++i)
		for(uint32 j = 0; j < MAX_COLUMNS; ++j)
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
	{
		// itr->first->ToString();
		printf("Pareja %d: {%d, %d}\n", ++total, itr->first.first, itr->first.second);
	}
}

std::vector<std::vector<char>>  sBoard(MAX_ROWS, std::vector<char>(MAX_COLUMNS)); // Tablero del juego
/*Point**/void InitializeMinesPositions(uint32 totalMines)
{
	for(uint32 i = 0; i < totalMines; ++i)
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
	for(uint32 i = 0; i < MAX_ROWS; ++i)
	{
		for(uint32 j = 0; j < MAX_COLUMNS; ++j)
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

uint32 const MAX_MOVES = 8; // Unused

void CalcMinas()
{
	for(std::pair<uint32, uint32> pos : sMinesStore)
	{
		int x = pos.first;
		int y = pos.second;
		for(int i = x - 1; i < x + 2; ++i)
			for(int j = y - 1; j < y + 2; ++j)
			{
				if(x == i && y == j) continue;
				if(i >= 0 && j >= 0 && i < MAX_ROWS && j < MAX_COLUMNS)
					if(sBoard[i][j] != '*')
						sBoard[i][j]++;
			}
	}
}

 int main()
{
	InitializeRandom();
	InitializeAvailablePointsContainer(5,5);
	InitializeMinesPositions(MAX_MINAS);
	// InitializeBoard();
	CalcMinas();
	ShowBoard();
	//ShowAvailablePoints();

  	return 0;
}
