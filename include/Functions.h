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

/*Подсветка клетки*/
void drawHighlightRect(sf::RenderWindow& window, const Board& board, const int cellSize ,const int offSetX, const int offSetY);
//
std::array<sf::FloatRect, 4> getPromotionButtonsRect(int sqRow, int sqCol, float offSetX, float offSetY, float cellSize);
void drawPromotionPopup(sf::RenderWindow& window, const Board& board, const std::vector<sf::Sprite>& sprites, float offSetX, float offSetY, float cellSize);
bool handlePromotionClick(const sf::Vector2i& mousePos, Board& board, float offSetX, float offSetY, float cellSize);


#endif // FUNCTIONS_H