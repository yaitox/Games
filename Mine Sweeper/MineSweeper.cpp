#include <windows.h>
#include "Board.h"

/****************** Console system *******************/
CONSOLE_FONT_INFOEX m_consoleInfo;
HANDLE m_out = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE m_in = GetStdHandle(STD_INPUT_HANDLE);
INPUT_RECORD m_ir;
DWORD m_events;
COORD m_coord;
CONSOLE_CURSOR_INFO m_cursorInfo;
/****************************************************/

std::vector<Point*> sAvailablePointsStore;
std::vector<Point*> sMinesStore;

Point* playerMove;
Board* sBoard;
uint32 m_discovered;

uint8 MAX_COLUMNS;
uint8 MAX_ROWS;
uint8 MAX_MINES;

// Initialize the board based on difficulty.
void SetBoardSizeByDifficulty(GameDifficulty difficulty)
{
	switch (difficulty)
	{
		case GameDifficulty::Easy:
			MAX_COLUMNS = 8;
			MAX_ROWS = 8;
			MAX_MINES = 10;
			break;

		case GameDifficulty::Medium:
			MAX_COLUMNS = 16;
			MAX_ROWS = 16;
			MAX_MINES = 40;
			break;

		case GameDifficulty::Hard:
			MAX_COLUMNS = 30;
			MAX_ROWS = 16;
			MAX_MINES = 99;
			break;

		default:
			break;
	}
	sBoard = new Board(MAX_ROWS, MAX_COLUMNS);
}

// Random seed
inline void InitializeRandom() { std::srand((unsigned int)std::time(nullptr)); }

// Store all the points on container, this container is used later to generate random mines positions.
void InitializeAvailablePointsContainer()
{
	for (uint32 row = 0; row < MAX_ROWS; ++row)
		for (uint32 col = 0; col < MAX_COLUMNS; ++col)
		{
			Point* point = new Point(row, col);
			sBoard->AddPoint(point);
			sAvailablePointsStore.push_back(point);
		}
}

void ShowAvailablePoints()
{
	for (std::vector<Point*>::iterator itr = sAvailablePointsStore.begin(); itr != sAvailablePointsStore.end(); ++itr)
		(*itr)->ToString();
}

// Random mine positions generator
void InitializeMinesPositions()
{
	for (uint8 i = 0; i < MAX_MINES; ++i)
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
	if (!point || point->hasKnown || point->isMine || point->isFlag) 
		return;

	point->hasKnown = true;
	++m_discovered;
	int pointRow = (int)point->x;
	int pointCol = (int)point->y;

	// Discover around the point
	for (int row = pointRow - 1; row <= pointRow + 1; ++row)
		for (int col = pointCol - 1; col <= pointCol + 1; ++col)
			if (Point* nearPoint = sBoard->GetPoint(row, col))
				if (point->closeMines == 0)
					DiscoverPoint(nearPoint);
}

// TODO: this must be called once the player has lost. The update board must only update on the position that has been played.
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

				if (playerMove && !playerMove->isFlag && playerMove->isMine)
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
	std::cout << "Select difficulty"							<< std::endl 
			  << "0 - Easy: 8 rows, 8 columns, 10 mines."		<< std::endl
			  << "1 - Medium: 16 rows, 16 columns, 40 mines."	<< std::endl
		      << "2 - Hard: 30 rows, 16 columns, 99 mines."		<< std::endl;

	uint32 difficulty; std::cin >> difficulty;

	if (!IsValidDifficulty(difficulty))
	{
		system("cls");
		AskUserForDifficulty();
		return;
	}

	SetBoardSizeByDifficulty(static_cast<GameDifficulty>(difficulty));
}

void InitializeRandomMines()
{
	InitializeAvailablePointsContainer();
	InitializeMinesPositions();
}

Point* MakeMove(bool& isFlagMove)
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
					break;

				if (button & RIGHTMOST_BUTTON_PRESSED)
				{
					if (!point->hasKnown)
					{
						point->isFlag = !point->isFlag;
						isFlagMove = true;
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
	Sleep(50); // Prevent click spam (hack?)
	bool flagMove = false;
	Point* point = MakeMove(flagMove);

	if (!point || flagMove)
	{
		if(flagMove)
			sBoard->ShowBoard();

		AskUserForMove();
		return;
	}

	playerMove = point;
	DiscoverPoint(playerMove);
}

void InitializeGame(); // TODO: must be inside a class?

void PlayGame()
{
	sBoard->ShowBoard();

	if (!playerMove->isFlag && playerMove->isMine)
	{
		std::cout << "YOU LOSE!" << std::endl;
		system("pause");
		InitializeGame();
		return;
	}

	if (m_discovered == sBoard->GetColums() * sBoard->GetRows() - MAX_MINES) // TODO: create a function to calc this
	{
		for (Point* mine : sMinesStore)
			if (!mine->isFlag)
				mine->isFlag = true;

		sBoard->ShowBoard();
		std::cout << "YOU WIN!" << std::endl;
		system("pause");
		InitializeGame();
		return;
	}

	AskUserForMove();
	PlayGame();
}

void SetConsoleSize(DWORD font = 0, SHORT x = 0, SHORT y = 30)
{
	m_consoleInfo.cbSize = sizeof(m_consoleInfo);
	m_consoleInfo.nFont = font;
	m_consoleInfo.dwFontSize.X = x;                   // Width of each character in the font
	m_consoleInfo.dwFontSize.Y = y;                  // Height
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