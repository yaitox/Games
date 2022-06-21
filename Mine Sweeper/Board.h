#include "Defines.h"
#include <vector>

enum class GameDifficulty : uint8
{
	Easy,
	Medium,
	Hard,

	Max
};

struct Point
{
private:
	uint32 _x;	// X Coord
	uint32 _y; 	// Y Coord
	bool _isKnown; // Tells if the point is already known
	char _symbol; // NYI: is supposed to be the sysmbol that represents the point: number, * or -

	// Mines system
	uint32 _aroundMines; // Total mines around the point
	bool _isMine;

	// Flag system
	bool _isFlag;

public:
	Point(uint32 x, uint32 y) : _x(x), _y(y), _symbol('-'), _aroundMines(0), _isKnown(false), _isMine(false), _isFlag(false) { }

	void ToString()
	{
		std::cout << "Point set on:" << std::endl
			<< "Row: " << _x << ' '
			<< "Column: " << _y << std::endl;

		if (_isMine)
			std::cout << "The point is a mine." << std::endl;
		else
			std::cout << (_isKnown ? "The point is already known" : "The point is not already known")
			<< " and has " << _aroundMines << " mines around." << "\n\n";
	}

	bool IsKnown() { return _isKnown; }
	bool IsFlag() { return _isFlag; }
	bool IsMine() { return _isMine; }

	uint32 GetCoordX() { return _x; }
	uint32 GetCoordY() { return _y; }
	uint32 GetAroundMines() { return _aroundMines; }

	void IncrementAroundMines() { _aroundMines++; }
	void InstallMine() { _isMine = true; }
	void Discover() { _isKnown = true; }
	void SetFlag() { _isFlag = !_isFlag; }
};

class Board
{
private:
	std::vector<std::vector<Point*>> m_board;
	uint32 m_rows;
	uint32 m_columns;
	uint32 m_mines;
	uint32 m_discover;
	GameDifficulty m_difficulty;

public:
	Board(GameDifficulty difficulty);

	void AddPoint(Point* point);
	void CalcNearPointsFromMine(Point* mine);
	void ShowBoard();
	void IncrementDiscovered() { ++m_discover; }

	Point* GetPoint(int row, int col);

	uint32 GetRows() { return m_rows; };
	uint32 GetColums() { return m_columns; }
	uint32 GetMines() { return m_mines; }
	uint32 GetTotalDiscovered() { return m_discover; }
	uint32 GetSize() { return GetRows() * GetColums(); }

	bool IsBoardDicovered();
	bool ContainsPoint(int row, int col);

	GameDifficulty GetDifficulty() { return m_difficulty; }
};
