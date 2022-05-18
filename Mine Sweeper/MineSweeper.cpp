#include <windows.h>
#include "Board.h"

/****************** Console *******************/
CONSOLE_FONT_INFOEX m_consoleInfo;
HANDLE m_out = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE m_in = GetStdHandle(STD_INPUT_HANDLE);
INPUT_RECORD m_ir;
DWORD m_events;
COORD m_coord;
CONSOLE_CURSOR_INFO m_cursorInfo;
/**********************************************/

std::vector<Point*> sAvailablePointsStore; // Contenedor usado para sacar puntos randoms
std::vector<Point*> sMinesStore; // Posiciones de las minas.

Point* playerMove;
Board* sBoard;
uint32 m_discovered;

uint8 MAX_COLUMNS;
uint8 MAX_ROWS;
uint8 MAX_MINAS;

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
		sMinesStore.push_back(mine);
		sBoard->CalcNearPointsFromMine(mine);

		sAvailablePointsStore.erase(itr);
	}
}

void DiscoverPoint(Point* point)
{
	if (!point)
		return;

	if (point->hasKnown || point->isMine || point->isFlag) return; // Punto ya explorado, mina o bandera no se chequea.

	point->hasKnown = true;
	m_discovered++;
	int pointRow = (int)point->x;
	int pointCol = (int)point->y;

	for (int row = pointRow - 1; row <= pointRow + 1; ++row)
		for (int col = pointCol - 1; col <= pointCol + 1; ++col)
			if (Point* nearPoint = sBoard->GetPoint(row, col))
				if (point->closeMines == 0) // Solo los 0 pueden descubrir varias casillas
					DiscoverPoint(nearPoint);
}

// TODO: solo deberia actualizar la posicion en la que ha dado clic. Esta funcion solo deberia llamarse cuando muere.
void Board::ShowBoard()
{
	system("cls");
	for (uint32 i = 0; i < MAX_ROWS; ++i)
	{
		for (uint32 j = 0; j < MAX_COLUMNS; ++j)
		{
			if (Point* point = sBoard->GetPoint(i, j))
			{
				if (point->isFlag)
				{
					std::cout << "F ";
					continue;
				}

				if (playerMove && !playerMove->isFlag && playerMove->isMine) // El player pierde, mostramos todo el tablero.
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

	uint32 diff; std::cin >> diff;

	if (!IsValidDifficulty(diff))
	{
		system("cls");
		AskUserForDifficulty();
		return;
	}

	SetBoardSizeByDifficulty(static_cast<GameDifficulty>(diff));
}

void InitializeRandomMines()
{
	InitializeAvailablePointsContainer();
	InitializeMinesPositions();
}

Point* MakeMove(bool& flag)
{
	m_cursorInfo.bVisible = false;

	SetConsoleCursorInfo(m_out, &m_cursorInfo);
	SetConsoleMode(m_in, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	ReadConsoleInput(m_in, &m_ir, 1, &m_events);

	Point* point = nullptr;

	switch (m_ir.EventType)
	{
		case MOUSE_EVENT:
		{
			DWORD button = m_ir.Event.MouseEvent.dwButtonState;
			
			if (button & (FROM_LEFT_1ST_BUTTON_PRESSED | RIGHTMOST_BUTTON_PRESSED))
			{
				point = sBoard->GetPoint(m_ir.Event.MouseEvent.dwMousePosition.Y, m_ir.Event.MouseEvent.dwMousePosition.X / 2);
				if (!point)
					break; // Rompemos si es nulo.

				if (button & RIGHTMOST_BUTTON_PRESSED)
				{
					if (!point->hasKnown)
					{
						point->isFlag = !point->isFlag; // Negamos la bandera. Si la tiene puesta, no la tendra. Si no la tiene puesta, la tendra.
						flag = true; // Helper
					}
				}	 

				SetConsoleCursorPosition(m_out, m_coord);
			}
		}

		FlushConsoleInputBuffer(m_in);
	}

	return point;
}

void AskUserForMove()
{
	Sleep(50);
	bool flag = false;
	Point* point = MakeMove(flag);

	if (!point || flag) // Si es nulo el point (cuando esta por fuera del tablero) o es un movimiento de bandera, seguimos pidiendo un movimiento.
	{
		if(flag)
			sBoard->ShowBoard();
		AskUserForMove();
		return;
	}

	playerMove = point;
	DiscoverPoint(playerMove);
}

void InitializeGame();

// Siempre sigue esta logica: pedimos, registramos, mostramos.
void PlayGame()
{
	sBoard->ShowBoard();

	if (!playerMove->isFlag && playerMove->isMine)
	{
		std::cout << "HAS PERDIDO!" << std::endl;
		system("pause");
		InitializeGame();
		return;
	}

	if (m_discovered == sBoard->GetColums() * sBoard->GetRows() - MAX_MINAS)
	{
		for (Point* mine : sMinesStore)
			if (!mine->isFlag)
				mine->isFlag = true;

		sBoard->ShowBoard();
		std::cout << "HAS GANADO!" << std::endl;
		system("pause");
		InitializeGame();
		return;
	}

	AskUserForMove();
	PlayGame();
}

void SetConsoleSize()
{
	m_consoleInfo.cbSize = sizeof(m_consoleInfo);
	m_consoleInfo.nFont = 0;
	m_consoleInfo.dwFontSize.X = 0;                   // Width of each character in the font
	m_consoleInfo.dwFontSize.Y = 30;                  // Height
	m_consoleInfo.FontFamily = FF_DONTCARE;
	m_consoleInfo.FontWeight = FW_NORMAL;
	SetCurrentConsoleFontEx(m_out, false, &m_consoleInfo);
}

void InitializeGame()
{
	playerMove = nullptr;
	sBoard = nullptr;
	m_discovered = 0;
	sAvailablePointsStore.clear();
	sMinesStore.clear();

	InitializeRandom();
	AskUserForDifficulty();
	InitializeRandomMines();
	sBoard->ShowBoard();
	AskUserForMove();
	PlayGame();
}

int main()
{
	SetConsoleSize();
	InitializeGame();
	return 0;
}
