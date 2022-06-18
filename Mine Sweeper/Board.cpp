#include "Board.h"

Board::Board(GameDifficulty difficulty)
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
	m_board.resize(m_rows, std::vector<Point*>(m_columns));
	m_difficulty = difficulty;
}

bool Board::ContainsPoint(int row, int col)
{
	if (row >= 0 && col >= 0 && row < (int)GetRows() && col < (int)GetColums())
		return true;
	return false;
}

void Board::AddPoint(Point* newPoint)
{
	m_board[newPoint->x][newPoint->y] = newPoint;
}

Point* Board::GetPoint(int row, int col)
{
	return (ContainsPoint(row, col) ? m_board[row][col] : nullptr);
}

void Board::CalcNearPointsFromMine(Point* mine)
{
	int mineRow = (int)mine->x;
	int mineCol = (int)mine->y;

	for (int row = mineRow - 1; row <= mineRow + 1; ++row)
		for (int col = mineCol - 1; col <= mineCol + 1; ++col)
			if (Point* nearPoint = GetPoint(row, col))
				if (!nearPoint->isMine)
					nearPoint->closeMines++;
}
