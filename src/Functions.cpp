#include "Functions.h"
#include <algorithm>

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

void drawHighlightRect(sf::RenderWindow& window, const Board& board, const int cellSize, const int offSetX, const int offSetY)
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

// Возвращает прямоугольники 4 кнопок попапа превращения.
// sqRow/sqCol — клетка, где стоит пешка.
// Возвращает в порядке: Q, R, B, N (сверху вниз).
std::array<sf::FloatRect, 4> getPromotionButtonsRect(int sqRow, int sqCol, float offSetX, float offSetY, float cellSize)
{
   // Позиция клетки превращения
   float sqX = offSetX + sqCol * cellSize;
   float sqY = offSetY + sqRow * cellSize;

   // По умолчанию попап — справа от клетки, столбиком вниз.
   float popupX = sqX + cellSize;
   float popupY = sqY;

   // Если не влезает справа — ставим слева.
   float boardRight = offSetX + cellSize * 8;
   if (popupX + cellSize > boardRight) {
      popupX = sqX - cellSize;
   }

   // Если не влезает снизу — сдвигаем вверх.
   float boardBottom = offSetY + cellSize * 8;
   if (popupY + cellSize * 4 > boardBottom) {
      popupY = boardBottom - cellSize * 4;
   }

   std::array<sf::FloatRect, 4> rects;
   for (int i = 0; i < 4; ++i) {
      rects[i] = sf::FloatRect({ popupX, popupY + i * cellSize },
         { cellSize, cellSize });
   }
   return rects;
}

void drawPromotionPopup(sf::RenderWindow& window, const Board& board, const std::vector<sf::Sprite>& sprites, float offSetX, float offSetY, float cellSize)
{
   if (!board.isWaitingForPromotion()) return;

   auto rects = getPromotionButtonsRect(board.getPromotionRow(),
      board.getPromotionCol(),
      offSetX, offSetY, cellSize);

   // Символы фигур для отрисовки. Цвет — тот, что сейчас ходит.
   // Если сейчас ход белых — пешка белая дошла до 8-й линии.
   bool white = board.getOrderMoves();

   const char pieces[4] = { 'Q', 'R', 'B', 'N' };

   for (int i = 0; i < 4; ++i) {
      // Фон кнопки
      sf::RectangleShape bg(rects[i].size);
      bg.setPosition(rects[i].position);
      bg.setFillColor(sf::Color(50, 50, 50, 230));
      bg.setOutlineColor(sf::Color::White);
      bg.setOutlineThickness(2.f);
      window.draw(bg);

      // Иконка фигуры — берём из уже загруженного атласа
      char symbol = white ? pieces[i]
         : static_cast<char>(std::tolower(pieces[i]));
      int idx = getSpriteIndex(symbol);
      if (idx >= 0) {
         sf::Sprite icon = sprites[idx];
         // Позиционируем спрайт так же, как это делает drawFigures
         icon.setPosition(rects[i].position);
         window.draw(icon);
      }
   }
}

// true — клик поглощён попапом (или попап был открыт).
bool handlePromotionClick(const sf::Vector2i& mousePos, Board& board, float offSetX, float offSetY, float cellSize)
{
   if (!board.isWaitingForPromotion()) return false;

   auto rects = getPromotionButtonsRect(board.getPromotionRow(),
      board.getPromotionCol(),
      offSetX, offSetY, cellSize);

   const char pieces[4] = { 'Q', 'R', 'B', 'N' };
   for (int i = 0; i < 4; ++i) {
      if (rects[i].contains(sf::Vector2f(mousePos))) {
         board.confirmPromotion(pieces[i]);
         return true;
      }
   }
   // Клик мимо попапа — поглощаем, чтобы не выбралась фигура на доске.
   return true;
}