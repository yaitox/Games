#include <SFML/Graphics.hpp>
#include "Board.h"

std::vector<Point*> sAvailablePointsStore;
std::vector<Point*> sMinesStore;

Board* sBoard;

namespace MineSweeper
{
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
		for (uint8 i = 0; i < sBoard->GetTotalMines(); ++i)
		{
			std::vector<Point*>::iterator itr = sAvailablePointsStore.begin();
			std::advance(itr, std::rand() % sAvailablePointsStore.size());

			Point* mine = *itr;
			mine->InstallMine();
			sBoard->CalcNearPointsFromMine(mine);
			sMinesStore.push_back(mine);

			sAvailablePointsStore.erase(itr);
		}
	}

	// Random seed
	inline void InitializeRandom()
	{
		unsigned int uTimeNow = unsigned int(std::time(0));
		std::srand(uTimeNow);
	}

	// Initialize the board based on difficulty.
	void SetBoardSizeByDifficulty(GameDifficulty difficulty)
	{
		sBoard = new Board(difficulty);
	}

	bool IsValidDifficulty(int difficulty)
	{
		return difficulty >= uint8(GameDifficulty::Easy) && difficulty <= uint8(GameDifficulty::Hard);
	}

	int ConvertCharToInt(char ch)
	{
		return ch - '0';
	}

	void AskUserForDifficulty()
	{
		std::cout << "Select difficulty" << std::endl
			<< "0 - Easy: 8 rows, 8 columns, 10 mines." << std::endl
			<< "1 - Medium: 16 rows, 16 columns, 40 mines." << std::endl
			<< "2 - Hard: 30 rows, 16 columns, 99 mines." << std::endl;

		std::string difficultyInput; std::cin >> difficultyInput; // We assume a string is given.

		int difficulty = ConvertCharToInt(difficultyInput[0]);

		if (difficultyInput.size() != 1 || !IsValidDifficulty(difficulty))
		{
			system("cls");
			AskUserForDifficulty();
			return;
		}

		SetBoardSizeByDifficulty(GameDifficulty(difficulty));
	}

	void InitializeRandomMines()
	{
		InitializeAvailablePointsContainer();
		InitializeMinesPositions();
	}

	uint32 GetWindowSize(uint32 rows)
	{
		uint32 size = 0;
		switch (rows)
		{
		case 8:
			size = 256;
			break;

		case 16:
			size = 512;
			break;

		case 30:
			size = 960;
			break;
		}

		return size;
	}

	void InitializeGame()
	{
		system("cls");
		delete sBoard;

		sBoard = nullptr;
		sAvailablePointsStore.clear();
		sMinesStore.clear();

		InitializeRandom();
		AskUserForDifficulty();
		InitializeRandomMines();
	}

	void PlayGame()
	{
		sf::RenderWindow window(sf::VideoMode(GetWindowSize(sBoard->GetColums()), GetWindowSize(sBoard->GetRows())), "Mine Sweeper");
		sf::Texture texture;
		texture.loadFromFile("tiles.jpg");
		sf::Sprite sprite(texture);
		sBoard->Update(window, sprite);

		while (window.isOpen())
		{
			sf::Event e;
			while (window.waitEvent(e))
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				int x = mousePos.x / 32;
				int y = mousePos.y / 32;

				switch (e.type)
				{
					case sf::Event::Closed:
						window.close();
						break;

					case sf::Event::MouseButtonPressed:
					{
						Point* point = sBoard->GetPoint(y, x);
						switch (e.key.code)
						{
							case sf::Mouse::Left:
								sBoard->DiscoverPoint(point);
								sBoard->Update(window, sprite);
								break;

							case sf::Mouse::Right:
								point->SetFlag();
								sBoard->Update(window, sprite);
								break;
						}
					}
				}
			}

			InitializeGame();
			PlayGame();
		}
	}
}

int main()
{
	MineSweeper::InitializeGame();
	MineSweeper::PlayGame();
	return 0;
}