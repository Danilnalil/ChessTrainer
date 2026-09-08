#pragma once

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <array>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Board.h"

/*Функция генерирует вектор спрайтов*/
// 1 - текстура, 2 - число колонок, 3 - число строк
std::vector<sf::Sprite> splitTextureIntoSprite( const sf::Texture& texture,
                                                int columns,
                                                int rows,
                                                float cellSize,
                                                bool keepAspectRatio = true
);  

/*Функция возвращает индекс, получая символ фигуры на доске*/
int getSpriteIndex(char piece);

/*Функция рисует доску*/
void drawFigures
(
   sf::RenderWindow& window,
   const std::array<std::array<char, 8>, 8>& board,
   const std::vector<sf::Sprite>& sprites,
   float offsetX, // начальное положение доски
   float offsetY,
   float cellSize // размер клетки
);

/*Функция перемещения фигуры*/
void movePiece(std::array<std::array<char, 8>, 8>& board, int row, int col, bool& isPieceSelected, int& selectedRow, int& selectedCol);
/*Подсветка клетки*/
void drawHighlightRect(sf::RenderWindow& window, const Board& board, const int cellSize ,const int offSetX, const int offSetY);

#endif // FUNCTIONS_H