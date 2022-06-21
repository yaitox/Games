#include "Board.h"

Board::Board(GameDifficulty difficulty) : m_difficulty(difficulty), m_discover(0)
{
	uint32 rows = 0, columns = 0, mines = 0;
	switch (difficulty)
	{
		case GameDifficulty::Easy:
			columns = 8;
			rows = 8;
			mines = 10;
			break;

		case GameDifficulty::Medium:
			columns = 16;
			rows = 16;
			mines = 40;
			break;

		case GameDifficulty::Hard:
			columns = 30;
			rows = 16;
			mines = 99;
			break;

		default:
			break;
	}

	m_rows = rows;
	m_columns = columns;
	m_mines = mines;
	m_board.resize(m_rows, std::vector<Point*>(m_columns));
}

bool Board::ContainsPoint(int row, int col)
{
	return (row >= 0 && col >= 0 && row < (int)GetRows() && col < (int)GetColums());
}

bool Board::IsBoardDicovered()
{
	return GetTotalDiscovered() == GetSize() - GetMines();
}

void Board::AddPoint(Point* newPoint)
{
	m_board[newPoint->GetCoordX()][newPoint->GetCoordY()] = newPoint;
}

Point* Board::GetPoint(int row, int col)
{
	return (ContainsPoint(row, col) ? m_board[row][col] : nullptr);
}

void Board::CalcNearPointsFromMine(Point* mine)
{
	int mineRow = (int)mine->GetCoordX();
	int mineCol = (int)mine->GetCoordY();

	for (int row = mineRow - 1; row <= mineRow + 1; ++row)
		for (int col = mineCol - 1; col <= mineCol + 1; ++col)
			if (Point* nearPoint = GetPoint(row, col))
				if (!nearPoint->IsMine())
					nearPoint->IncrementAroundMines();
}
