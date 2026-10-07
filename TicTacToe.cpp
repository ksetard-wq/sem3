#include "TicTacToe.h"
#include <iostream>

TicTacToe::TicTacToe(size_t sz, size_t win) {
    reset(sz, win);
}

void TicTacToe::reset(size_t sz, size_t win) {
    size = (sz > 10) ? 10 : (sz < 3 ? 3 : sz);
    win_len = (win > size) ? size : (win < 3 ? 3 : win);
    current = Cell::Cross;
    moves = 0;
    finished = false;

    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j)
            board[i][j] = Cell::Empty;
}

Cell TicTacToe::getCell(size_t r, size_t c) const {
    if (r >= size || c >= size) return Cell::Empty;
    return board[r][c];
}

Cell TicTacToe::getCurrentPlayer() const {
    return current;
}

bool TicTacToe::isFinished() const {
    return finished;
}

size_t TicTacToe::getSize() const {
    return size;
}

bool TicTacToe::makeMove(size_t r, size_t c) {
    if (finished || r >= size || c >= size || board[r][c] != Cell::Empty) return false;

    board[r][c] = current;
    moves++;

    if (checkWin(r, c)) {
        finished = true;
        std::cout << "\n>>> Игрок " << (current == Cell::Cross ? "X" : "O") << " ПОБЕДИЛ! <<<\n";
    } else if (static_cast<size_t>(moves) == size * size) {
        finished = true;
        std::cout << "\n>>> НИЧЬЯ! <<<\n";
    } else {
        current = (current == Cell::Cross) ? Cell::Nought : Cell::Cross;
    }
    return true;
}

bool TicTacToe::checkWin(size_t r, size_t c) {
    int dr[] = {0, 1, 1, 1};
    int dc[] = {1, 0, 1, -1};

    for (int d = 0; d < 4; ++d) {
        size_t count = 1;
        for (int dir = -1; dir <= 1; dir += 2) {
            for (size_t step = 1; step < win_len; ++step) {
                int nr = static_cast<int>(r) + dr[d] * static_cast<int>(step) * dir;
                int nc = static_cast<int>(c) + dc[d] * static_cast<int>(step) * dir;
                if (nr >= 0 && nr < static_cast<int>(size) && 
                    nc >= 0 && nc < static_cast<int>(size) && 
                    board[nr][nc] == current) {
                    count++;
                } else break;
            }
        }
        if (count >= win_len) return true;
    }
    return false;
}

void TicTacToe::print() const {
    std::cout << "\n  ";
    for (size_t j = 0; j < size; ++j) std::cout << j << " ";
    std::cout << "\n";

    for (size_t i = 0; i < size; ++i) {
        std::cout << i << " ";
        for (size_t j = 0; j < size; ++j) {
            char ch = '.';
            if (board[i][j] == Cell::Cross) ch = 'X';
            if (board[i][j] == Cell::Nought) ch = 'O';
            std::cout << ch << " ";
        }
        std::cout << "\n";
    }
}