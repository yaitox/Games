#include "Board.h"

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
