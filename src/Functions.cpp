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

// ===================== Плашка результата партии (мат / пат) =====================

// Геометрия плашки: по центру окна 1200x800.
static const sf::FloatRect gGameOverPanelRect({ 350.f, 290.f }, { 500.f, 220.f });

sf::FloatRect getGameOverResetButtonRect()
{
   // Кнопка «Сброс» — внизу плашки, по центру.
   const float btnW = 220.f;
   const float btnH = 54.f;
   const sf::Vector2f p = gGameOverPanelRect.position;
   const sf::Vector2f s = gGameOverPanelRect.size;
   return sf::FloatRect({ p.x + (s.x - btnW) / 2.f,
                          p.y + s.y - btnH - 20.f }, { btnW, btnH });
}

void drawGameOverPanel(sf::RenderWindow& window, const Board& board, const sf::Font& font)
{
   using GS = Board::GameState;
   const GS state = board.getGameState();
   if (state == GS::Playing) return;

   // Затемнение фона
   sf::RectangleShape dim({ { 1200.f, 800.f } });
   dim.setFillColor(sf::Color(0, 0, 0, 140));
   window.draw(dim);

   // Сама плашка
   sf::RectangleShape panel(gGameOverPanelRect.size);
   panel.setPosition(gGameOverPanelRect.position);
   panel.setFillColor(sf::Color(35, 35, 40, 245));
   panel.setOutlineColor(sf::Color(220, 180, 90));
   panel.setOutlineThickness(3.f);
   window.draw(panel);

   // Текст результата
   const wchar_t* title = L"";
   switch (state)
   {
      case GS::WhiteWins: title = L"\x041C\x0430\x0442! \x041F\x043E\x0431\x0435\x0434\x0430 \x0431\x0435\x043B\x044B\x0445"; break; // "Мат! Победа белых"
      case GS::BlackWins: title = L"\x041C\x0430\x0442! \x041F\x043E\x0431\x0435\x0434\x0430 \x0447\x0451\x0440\x043D\x044B\x0445"; break; // "Мат! Победа чёрных"
      case GS::Draw:      title = L"\x041F\x0430\x0442! \x041D\x0438\x0447\x044C\x044F";                                  break; // "Пат! Ничья"
      default: return;
   }

   sf::Text text(font);
   text.setString(title);
   text.setCharacterSize(34);
   text.setFillColor(sf::Color::White);
   text.setPosition({ gGameOverPanelRect.position.x,
                      gGameOverPanelRect.position.y + 30.f });
   // Центрируем заголовок по горизонтали плашки
   auto bounds = text.getLocalBounds();
   text.setOrigin({ bounds.position.x + bounds.size.x / 2.f, 0.f });
   text.setPosition({ gGameOverPanelRect.position.x + gGameOverPanelRect.size.x / 2.f,
                      gGameOverPanelRect.position.y + 30.f });
   window.draw(text);

   // Кнопка «Сброс»
   auto btnRect = getGameOverResetButtonRect();
   sf::RectangleShape button(btnRect.size);
   button.setPosition(btnRect.position);
   button.setFillColor(sf::Color(70, 130, 70));
   button.setOutlineColor(sf::Color::White);
   button.setOutlineThickness(2.f);
   window.draw(button);

   sf::Text btnText(font);
   btnText.setString(L"\x0421\x0431\x0440\x043E\x0441"); // "Сброс"
   btnText.setCharacterSize(26);
   btnText.setFillColor(sf::Color::White);
   auto b = btnText.getLocalBounds();
   btnText.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f });
   btnText.setPosition({ btnRect.position.x + btnRect.size.x / 2.f,
                         btnRect.position.y + btnRect.size.y / 2.f });
   window.draw(btnText);
}

bool handleGameOverClick(const sf::Vector2i& mousePos, Board& board)
{
   if (!board.isGameOver()) return false;

   // Клики по плашке всегда поглощаются (доска под ней заблокирована).
   if (gGameOverPanelRect.contains(sf::Vector2f(mousePos)))
   {
      if (getGameOverResetButtonRect().contains(sf::Vector2f(mousePos)))
      {
         board.init(); // полный сброс партии
      }
      return true;
   }
   return false;
}