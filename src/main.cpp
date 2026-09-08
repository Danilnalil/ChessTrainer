#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>
#include "Functions.h"
#include "Board.h"

int main() {
   sf::RenderWindow window(sf::VideoMode({ 1200, 800 }), "Шахматный тренажёр");
   window.setFramerateLimit(60);

   //------------------------------------------- Constants and bools

   const int boardSizePixel = 560;                 // размер доски в пикселях
   const int cellSizePixel = boardSizePixel / 8;   // размер одной клетки

   //------------------------------------------- value

   int highlightSelectedCol = -1; // подсвеченная клетка
   int highlightSelectedRow = -1;

   //------------------------------------------- Textures & Sprites

   sf::Texture texture;
   if (!texture.loadFromFile("C:/Projects/MyChessTrainer/assets/chess_fg.png"))
   {
      std::cout << "Exit in if (!texture.loadFromFile";
      return -1;
   }

   std::vector<sf::Sprite> sprites = splitTextureIntoSprite(texture, 6, 2, cellSizePixel, true);

   //------------------------------------------- Board

   Board chessBoard;

   int offSetX = 800 - boardSizePixel - 100; // начальное положение доски x
   int offSetY = 600 - boardSizePixel; // начальное положение доски y

   sf::Color Clear(240, 217, 181); // светло-бежевый
   sf::Color Dark (181, 136, 99); // тёмно-коричневый

   //------------------------------------------- Interactive
   

   //-------------------------------------------
   while (window.isOpen()) {
      while (const auto event = window.pollEvent()) {
         if (event->is<sf::Event::Closed>()) {
            window.close();
         }
         if (  const auto* eventMouse = event->getIf<sf::Event::MouseButtonPressed>(); 
               eventMouse && eventMouse->button == sf::Mouse::Button::Left()) 
         {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);  // получаю координаты мыши при нажатии (int)
            int col = (mousePos.x - offSetX) / cellSizePixel;        // вычисляем клетку, в которой сейчас мышь
            int row = (mousePos.y - offSetY) / cellSizePixel;

            bool order = chessBoard.getOrderMoves();
            chessBoard.handleClick(row, col, order); // обработка хода
         }
      }

      window.clear(sf::Color::Black);
      for (int row = 0; row < 8; row++) {
         for (int col = 0; col < 8; col++) {
            sf::RectangleShape rect({ cellSizePixel,cellSizePixel });
            rect.setPosition({ static_cast<float>(offSetX + (col * cellSizePixel)) , static_cast<float>(offSetY + (row * cellSizePixel)) });
            if ((row + col) % 2 == 0) { rect.setFillColor(Clear); }
            if ((row + col) % 2 != 0) { rect.setFillColor(Dark); }
            window.draw(rect);
         }
      }
      drawFigures(window, chessBoard.getBoard(), sprites, offSetX, offSetY, cellSizePixel);
      drawHighlightRect(window, chessBoard, cellSizePixel, offSetX, offSetY);
      window.display();
   }

   return 0;
}