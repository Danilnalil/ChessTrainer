#include "Functions.h"

std::vector<sf::Sprite> splitTextureIntoSprite(const sf::Texture& texture,  int columns,  int rows, float cellSize, bool keepAspectRatio)
{
   std::vector<sf::Sprite> sprites; // вектор спрайтов
   sprites.reserve(columns * rows); // резервирую память под спрайты

   sf::Vector2u textureSize = texture.getSize(); // Размер всей текстуры

   int spriteWidth = textureSize.x / columns  + 4; // Размер одного спрайта (предполагаем, что текстура делится ровно)
   int spriteHeight = textureSize.y / rows    -  2;
   // вычисляю масштаб
   float scaleX = cellSize / static_cast<float>(spriteWidth) + 0.1;
   float scaleY = cellSize / static_cast<float>(spriteHeight) + 0.1;
   float scale = 1.f;
   if (keepAspectRatio) { scale = std::min(scaleX, scaleY); }

   for (int row = 0; row < rows; ++row)
   {
      for (int col = 0; col < columns; ++col)
      {
         // Прямоугольник, соответствующий текущему фрагменту
         sf::IntRect rect({ col * spriteWidth + 3 * col , row * spriteHeight }, { spriteWidth, spriteHeight });

         // Создаю спрайт с этим фрагментом текстуры
         sf::Sprite sprite(texture, rect);
         if (keepAspectRatio) { sprite.setScale({ scale, scale }); }
         sprites.push_back(sprite);
      }
      
   }
   return sprites;
}

int getSpriteIndex(char piece)
{
   switch (piece)
   {
      // Чёрные фигуры (индексы 0..5)
   case 'k': return 0;  // король
   case 'q': return 1;  // ферзь
   case 'r': return 2;  // ладья
   case 'b': return 3;  // слон
   case 'n': return 4;  // конь
   case 'p': return 5;  // пешка

      // Белые фигуры (индексы 6..11)
   case 'K': return 6;
   case 'Q': return 7;
   case 'R': return 8;
   case 'B': return 9;
   case 'N': return 10;
   case 'P': return 11;

   default: return -1;  // пустая клетка или неизвестный символ
   }
}

void drawFigures(sf::RenderWindow& window, const std::array<std::array<char, 8>, 8>& board, const std::vector<sf::Sprite>& sprites, float offsetX, float offsetY, float cellSize)
{

   for (int row = 0; row < 8; ++row)
   {
      for (int col = 0; col < 8; ++col)
      {
         char piece = board[row][col];
         if (piece == '.') continue;  // пустая клетка

         int index = getSpriteIndex(piece);
         if (index < 0 || index >= static_cast<int>(sprites.size()))
            continue;  // некорректный символ

         // Создаём копию спрайта, чтобы не изменять оригинальный
         sf::Sprite sprite = sprites[index];
         // Устанавливаем позицию в центр клетки (по желанию можно просто поставить левый верхний угол)
         sprite.setPosition({ offsetX + col * cellSize , offsetY + row * cellSize });
         window.draw(sprite);
      }
   }
}

void movePiece(std::array<std::array<char, 8>, 8>& board, int row, int col, bool& isPieceSelected, int& selectedRow, int& selectedCol)
{
   static char pieceSymbol = '\0';

   // игнорируем некорректный клик
   if (row < 0 || row >= static_cast<int>(board.size()) ||
      col < 0 || col >= static_cast<int>(board[0].size())) 
   {
      return; 
   }
   if (!isPieceSelected) {
      // Если фигура не выбрана, пытаемся выбрать
      if (board[row][col] != '.') // на клетке есть фигура
      { 
         isPieceSelected = true;
         selectedRow = row;
         selectedCol = col;
         pieceSymbol = board[row][col];
      }
      // Если клетка пуста – ничего не делать
   }
   else 
   {
      // Фигура уже выбрана
      if (row == selectedRow && col == selectedCol)  // Клик по той же клетке – отменяем выбор
      {  
         isPieceSelected = false;
      }
      else 
      {
         // Клик по другой клетке – перемещаем фигуру
         board[selectedRow][selectedCol] = '.';    // освобождаем старую клетку
         board[row][col] = pieceSymbol;            // ставим фигуру на новую
         isPieceSelected = false;                  // Сброс состояние выбора
      }
   }
}

void drawHighlightRect(sf::RenderWindow& window, const Board& board, const int cellSize, const int offSetX,
   const int offSetY)
{
   if (board.isPieceSelected())
   {
      static sf::RectangleShape highlightRect;                           // прямоугольник-подсветка
      highlightRect.setSize(sf::Vector2f(cellSize, cellSize));    // размер прямоугольника
      highlightRect.setFillColor(sf::Color(255, 255, 0, 80));     // полупрозрачный жёлтый
      highlightRect.setOutlineColor(sf::Color::Yellow);
      highlightRect.setOutlineThickness(2.f);                     // толщина рамки

      float coordX_highlightRect = offSetX + board.getSelectedCol() * cellSize;
      float coordY_highlightRect = offSetY + board.getSelectedRow() * cellSize;
      highlightRect.setPosition({ coordX_highlightRect , coordY_highlightRect });
      window.draw(highlightRect);
   }
}

