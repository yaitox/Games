#include <windows.h>
// #include "Board.h"
#include "Player.h"

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
// Player* player; Future use

// Initialize the board based on difficulty.
void SetBoardSizeByDifficulty(GameDifficulty difficulty)
{
	sBoard = new Board(difficulty);
}

// Random seed
inline void InitializeRandom() { std::srand((unsigned int)std::time(nullptr)); }

// Store all the points on container, this container is used later to generate random mines positions.
void InitializeAvailablePointsContainer()
{
	for (uint32 row = 0; row < sBoard->GetRows(); ++row)
		for (uint32 col = 0; col < sBoard->GetColums(); ++col)
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
	for (uint8 i = 0; i < sBoard->GetMines(); ++i)
	{
		std::vector<Point*>::iterator itr = sAvailablePointsStore.begin();
		std::advance(itr, std::rand() % sAvailablePointsStore.size());

		Point* mine = *itr;
		mine->InstallMine();
		sMinesStore.push_back(mine);
		sBoard->CalcNearPointsFromMine(mine);

		sAvailablePointsStore.erase(itr);
	}
}

void DiscoverPoint(Point* point)
{
	if (!point || point->IsKnown() || point->IsMine() || point->IsFlag()) 
		return;

	point->Discover();
	sBoard->IncrementDiscovered();
	int pointRow = (int)point->GetCoordX();
	int pointCol = (int)point->GetCoordY();

	// Discover around the point
	for (int row = pointRow - 1; row <= pointRow + 1; ++row)
		for (int col = pointCol - 1; col <= pointCol + 1; ++col)
			if (Point* nearPoint = sBoard->GetPoint(row, col))
				if (point->GetAroundMines() == 0)
					DiscoverPoint(nearPoint);
}

// TODO: this must be called once the player has lost. The update board must only update on the position that has been played.
bool stillPlaying;
bool playerWin;
void Board::ShowBoard()
{
	system("cls");
	for (uint32 i = 0; i < sBoard->GetRows(); ++i)
	{
		for (uint32 j = 0; j < sBoard->GetColums(); ++j)
		{
			if (Point* point = sBoard->GetPoint(i, j))
			{
				if (stillPlaying)
				{
					if (point->IsFlag())
						std::cout << 'F';
					else if (point->IsKnown())
						std::cout << point->GetAroundMines();
					else
						std::cout << '-';
				}
				else
				{
					
					if (point->IsMine())
					{
						if (playerWin)
							std::cout << 'F';
						else
							std::cout << '*';
					}	
					else
						std::cout << point->GetAroundMines();
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

int ConvertCharToInt(char ch)
{
	return ch - '0';
}

void AskUserForDifficulty()
{
	std::cout << "Select difficulty"							<< std::endl 
			  << "0 - Easy: 8 rows, 8 columns, 10 mines."		<< std::endl
			  << "1 - Medium: 16 rows, 16 columns, 40 mines."	<< std::endl
		      << "2 - Hard: 30 rows, 16 columns, 99 mines."		<< std::endl;

	std::string difficultyInput; std::cin >> difficultyInput; // We assume a string is given.

	if (difficultyInput.size() != 1 || !IsValidDifficulty(ConvertCharToInt(difficultyInput[0])))
	{
		system("cls");
		AskUserForDifficulty();
		return;
	}

	SetBoardSizeByDifficulty(static_cast<GameDifficulty>(ConvertCharToInt(difficultyInput[0])));
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

				if (point->IsKnown())
					return nullptr;

				if (button == FROM_LEFT_1ST_BUTTON_PRESSED)		
					if (point->IsFlag())
						return nullptr; // Invalid left click if the position is marked as flag.

				if (button == RIGHTMOST_BUTTON_PRESSED)
				{
					if (!point->IsKnown())
					{
						point->SetFlag();
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
	// player->SetMaxScore(sBoard->GetDifficulty(), m_discovered); Future use

	if (playerMove->IsMine())
		stillPlaying = false;

	if(sBoard->IsBoardDicovered())
	{
		stillPlaying = false;
		playerWin = true;
	}

	sBoard->ShowBoard();

	if (!stillPlaying)
	{
		std::cout << (playerWin ? "YOU WIN!" : "YOU LOSE!") << std::endl;
		system("pause");
		system("cls");
		InitializeGame();
		return;
	}

	else
	{
		AskUserForMove();
		PlayGame();
	}
}

void SetConsoleSize(DWORD font = 0, SHORT x = 0, SHORT y = 30)
{
	m_consoleInfo.cbSize = sizeof(m_consoleInfo);
	m_consoleInfo.nFont = font;
	m_consoleInfo.dwFontSize.X = x;                  // Width of each character in the font
	m_consoleInfo.dwFontSize.Y = y;                  // Height
	m_consoleInfo.FontFamily = FF_DONTCARE;
	m_consoleInfo.FontWeight = FW_NORMAL;
	SetCurrentConsoleFontEx(m_out, false, &m_consoleInfo);
}

void InitializeGame()
{
	delete sBoard;
	delete playerMove;

	playerMove = nullptr;
	sBoard = nullptr;
	stillPlaying = true;
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
	// player = new Player(1, "Pepito"); Future use
	SetConsoleSize();
	InitializeGame();
	return 0;
}