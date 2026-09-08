#include "Board.h"

Board::Board() : pieceSelected(false), selectedRow(-1), selectedCol(-1), selectedPiece('\0') {
   init();
}

void Board::init() {
   // Начальная расстановка (снизу вверх, как в шахматной нотации):
   // 8-я линия (ряд 0) – чёрные фигуры
   board[0] = { 'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r' };
   // 7-я линия (ряд 1) – чёрные пешки
   board[1] = { 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p' };

   // Ряды 2–5 – пустые клетки (обозначим точкой или пробелом)
   for (int i = 2; i <= 5; ++i) {
      board[i].fill('.');  // или ' ' – символ пустой клетки
   }

   // 2-я линия (ряд 6) – белые пешки
   board[6] = { 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P' };
   // 1-я линия (ряд 7) – белые фигуры
   board[7] = { 'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R' };

   // Сброс состояния выбора
   pieceSelected = false;
   selectedRow = -1;
   selectedCol = -1;
   selectedPiece = '\0';
   // стартовые позиции королей
   whiteKingRow = 7;
   whiteKingCol = 4;
   blackKingRow = 0;
   blackKingCol = 4;
}

void Board::handleClick(int row, int col, bool& orderMoves) {

   // Игнорируем клик вне доски
   if (row < 0 || row >= 8 || col < 0 || col >= 8) {
      return;
   }
   
   if (!pieceSelected) {
      // Попытка выбрать фигуру

      if (board[row][col] != '.') {
         char piece = board[row][col];
         bool flag = std::isupper(static_cast<unsigned char>(piece));
         if ( flag != orderMoves) { return; }
         legalMoves = generateLegalMoves(row, col);
         pieceSelected = true;
         selectedRow = row;
         selectedCol = col;
         selectedPiece = board[row][col];
      }
   }
   else {
      // Фигура уже выбрана
      if (row == selectedRow && col == selectedCol) {
         // Повторный клик по той же клетке – отмена выбора
         pieceSelected = false;
         selectedRow = -1;
         selectedCol = -1;
         selectedPiece = '\0';
      }
      else {
         // Перемещение фигуры
         if (isLegalMove(legalMoves, selectedRow, selectedCol, row, col)) 
         {
            makeMove(selectedRow, selectedCol, row, col, selectedPiece);
         }
         else {
            pieceSelected = false;
            selectedRow = -1;
            selectedCol = -1;
            selectedPiece = '\0';
         }
         
      }
   }
}

std::vector<Board::Move> Board::generatePseudoLegalMoves(int row, int col) const
{
   std::vector<Move> moves;
   moves.reserve(32); // максимально возможное число ходов (ферзь в открытой позиции)

   // Проверка границ
   if (row < 0 || row >= 8 || col < 0 || col >= 8) { return moves; }


   char piece = board[row][col];          // текущая фигура
   if (piece == '.') { return moves; }    // нажатие на пустую клетку

   char pieceUpper = std::toupper(static_cast<unsigned char>(piece));   // Кладём заглавную букву св стиле С
   bool isWhite = std::isupper(static_cast<unsigned char>(piece));      // смотрим, заглавная или маленькая буква
  
   // Лямбда для проверки границ
   auto isInside = [](int r, int c) {
      return r >= 0 && r < 8 && c >= 0 && c < 8;
      };

   // Лямбда для проверки, является ли клетка вражеской или пустой
   auto canMoveTo = [&](int r, int c) {
      if (!isInside(r, c)) { return false; }
      char target = board[r][c];
      return target == '.' || (isWhite != std::isupper(static_cast<unsigned char>(target)));
      };

   switch (pieceUpper) {
   case 'P': { // Пешка
      int dir = isWhite ? -1 : 1;
      int startRow = isWhite ? 6 : 1;

      // Ход вперёд
      int newRow = row + dir;
      int newCol = col;
      if (isInside(newRow, newCol) && board[newRow][newCol] == '.') {
         moves.push_back({ col, row, newCol, newRow });

         // Двойной ход с начальной позиции
         if (row == startRow) {
            int newRow2 = row + 2 * dir;
            int newCol2 = col;
            if (board[newRow2][newCol2] == '.') {
               moves.push_back({ col, row, newCol2, newRow2 });
            }
         }
      }

      // Взятия по диагоналям
      int captureRow = row + dir;
      for (int dcol = -1; dcol <= 1; dcol += 2) {
         int captureCol = col + dcol;
         if (isInside(captureRow, captureCol)) {
            char target = board[captureRow][captureCol];
            if (target != '.' && (isWhite != std::isupper(static_cast<unsigned char>(target)))) {
               moves.push_back({ col, row, captureCol, captureRow });
            }
         }
      }
      break;
   }

   case 'N': { // Конь
      const int offsets[8][2] = {
          {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
          {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
      };
      for (auto& off : offsets) {
         int toRow = row + off[0];
         int toCol = col + off[1];
         if (canMoveTo(toRow, toCol)) {
            moves.push_back({ col, row, toCol, toRow });
         }
      }
      break;
   }

   case 'B': { // Слон
      const int dirs[4][2] = {
          {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
      };
      for (auto& d : dirs) {
         int toRow = row + d[0];
         int toCol = col + d[1];
         while (isInside(toRow, toCol)) {
            char target = board[toRow][toCol];
            if (target == '.') {
               moves.push_back({ col, row, toCol, toRow });
            }
            else {
               if (isWhite != std::isupper(static_cast<unsigned char>(target))) {
                  moves.push_back({ col, row, toCol, toRow });
               }
               break;
            }
            toRow += d[0];
            toCol += d[1];
         }
      }
      break;
   }

   case 'R': { // Ладья
      const int dirs[4][2] = {
          {1, 0}, {-1, 0}, {0, 1}, {0, -1}
      };
      for (auto& d : dirs) {
         int toRow = row + d[0];
         int toCol = col + d[1];
         while (isInside(toRow, toCol)) {
            char target = board[toRow][toCol];
            if (target == '.') {
               moves.push_back({ col, row, toCol, toRow });
            }
            else {
               if (isWhite != std::isupper(static_cast<unsigned char>(target))) {
                  moves.push_back({ col, row, toCol, toRow });
               }
               break;
            }
            toRow += d[0];
            toCol += d[1];
         }
      }
      break;
   }

   case 'Q': { // Ферзь (слон + ладья)
      const int dirs[8][2] = {
          {1, 0}, {-1, 0}, {0, 1}, {0, -1},
          {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
      };
      for (auto& d : dirs) {
         int toRow = row + d[0];
         int toCol = col + d[1];
         while (isInside(toRow, toCol)) {
            char target = board[toRow][toCol];
            if (target == '.') {
               moves.push_back({ col, row, toCol, toRow });
            }
            else {
               if (isWhite != std::isupper(static_cast<unsigned char>(target))) {
                  moves.push_back({ col, row, toCol, toRow });
               }
               break;
            }
            toRow += d[0];
            toCol += d[1];
         }
      }
      break;
   }

   case 'K': { // Король
      const int offsets[8][2] = {
          {1, 0}, {-1, 0}, {0, 1}, {0, -1},
          {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
      };
      for (auto& off : offsets) {
         int toRow = row + off[0];
         int toCol = col + off[1];
         if (canMoveTo(toRow, toCol)) {
            moves.push_back({ col, row, toCol, toRow });
         }
      }
      break;
   }

   default:
      break; // неизвестная фигура
   }

   return moves;
}

std::vector<Board::Move> Board::generateLegalMoves(int row, int col) const
{
   auto pseudo = generatePseudoLegalMoves(row, col); 
   std::vector<Move> legal;
   legal.reserve(pseudo.size());
   for (const Move& m : pseudo) {
      if (isMoveLegal(m, this->orderMoves)) {
         legal.push_back(m);
      }
   }
   return legal;
}

bool Board::isLegalMove(const std::vector<Move>& legalMoves, int selectedRow, int selectedCol, int toRow, int toCol) const
{
   for (const Move& move : legalMoves) {
      if (move.fromRow == selectedRow && move.fromCol == selectedCol &&
         move.toRow == toRow && move.toCol == toCol)
      {
         return true;
      }
   }
   return false;
}

bool Board::isMoveLegal(const Move& move, bool isWhite) const
{
   // 1. Копирование доски
   char tempBoard[8][8];
   std::copy(&board[0][0], &board[0][0] + 64, &tempBoard[0][0]);

   // 2. Копир. координаты королей
   int tempWhiteKingRow = whiteKingRow;
   int tempWhiteKingCol = whiteKingCol;
   int tempBlackKingRow = blackKingRow;
   int tempBlackKingCol = blackKingCol;

   // 3. Выполнение хода на временной доске (обновляя координаты королей)
   char piece = tempBoard[move.fromRow][move.fromCol];
   tempBoard[move.toRow][move.toCol] = piece;
   tempBoard[move.fromRow][move.fromCol] = '.';

   // Если ходит король — обновляю его координаты
   char upper = std::toupper(piece);
   if (upper == 'K') {
      if (std::isupper(piece)) { // белый король
         tempWhiteKingRow = move.toRow;
         tempWhiteKingCol = move.toCol;
      }
      else { // чёрный король
         tempBlackKingRow = move.toRow;
         tempBlackKingCol = move.toCol;
      }
   }

   // 4. Теперь беру координаты короля нужного цвета
   int kingRow, kingCol;
   if (isWhite) {
      kingRow = tempWhiteKingRow;
      kingCol = tempWhiteKingCol;
   }
   else {
      kingRow = tempBlackKingRow;
      kingCol = tempBlackKingCol;
   }

   // 5. Проверяю атаку на этой клетке
   bool attacked = isSquareAttacked(tempBoard, kingRow, kingCol, !isWhite);
   return !attacked;
}

bool Board::isSquareAttacked(const char board[8][8], int row, int col, bool byWhite) const
{
   // Вспомогательная лямбда для проверки, что клетка содержит фигуру нужного цвета
   auto isEnemy = [&](int r, int c) -> bool {
      if (r < 0 || r >= 8 || c < 0 || c >= 8) return false;
      char p = board[r][c];
      if (p == '.') return false;
      bool isWhitePiece = std::isupper(static_cast<unsigned char>(p));
      return isWhitePiece == byWhite; // true, если цвет совпадает с атакующим
      };

   // 1. Проверка линий (ладья, слон, ферзь)
   // Направления: {dr, dc}
   const int dirs[8][2] = {
       {1,0}, {-1,0}, {0,1}, {0,-1},  // вертикали и горизонтали
       {1,1}, {1,-1}, {-1,1}, {-1,-1} // диагонали
   };
   for (int d = 0; d < 8; ++d) {
      int r = row + dirs[d][0];
      int c = col + dirs[d][1];
      while (r >= 0 && r < 8 && c >= 0 && c < 8) {
         char p = board[r][c];
         if (p == '.') {
            r += dirs[d][0];
            c += dirs[d][1];
            continue;
         }
         // Нашли фигуру
         if (isEnemy(r, c)) {
            char upper = std::toupper(static_cast<unsigned char>(p));
            // Для вертикалей/горизонталей (d < 4) – ладья или ферзь
            if (d < 4 && (upper == 'R' || upper == 'Q')) return true;
            // Для диагоналей (d >= 4) – слон или ферзь
            if (d >= 4 && (upper == 'B' || upper == 'Q')) return true;
         }
         break; // дальше не идём, т.к. наткнулись на фигуру (свою или чужую, но не ту, что нам нужна)
      }
   }

   // 2. Проверка коней (8 позиций)
   const int knightOffsets[8][2] = {
       {2,1}, {2,-1}, {-2,1}, {-2,-1},
       {1,2}, {1,-2}, {-1,2}, {-1,-2}
   };
   for (auto& off : knightOffsets) {
      int r = row + off[0];
      int c = col + off[1];
      if (isEnemy(r, c)) {
         char p = board[r][c];
         char upper = std::toupper(static_cast<unsigned char>(p));
         if (upper == 'N') return true;
      }
   }

   // 3. Проверка пешек (диагонали вперёд)
   if (byWhite) { // атакуют белые пешки (значит они бьют вниз по доске, т.е. row+1)
      int r = row + 1;
      if (r < 8) {
         for (int dc = -1; dc <= 1; dc += 2) {
            int c = col + dc;
            if (c >= 0 && c < 8 && board[r][c] == 'P') return true;
         }
      }
   }
   else { // атакуют чёрные пешки (бьют вверх, row-1)
      int r = row - 1;
      if (r >= 0) {
         for (int dc = -1; dc <= 1; dc += 2) {
            int c = col + dc;
            if (c >= 0 && c < 8 && board[r][c] == 'p') return true;
         }
      }
   }

   // 4. (Опционально) Проверка короля – если рядом стоит вражеский король (не должно случаться в легальной позиции)
   const int kingOffsets[8][2] = {
       {1,0}, {-1,0}, {0,1}, {0,-1},
       {1,1}, {1,-1}, {-1,1}, {-1,-1}
   };
   for (auto& off : kingOffsets) {
      int r = row + off[0];
      int c = col + off[1];
      if (isEnemy(r, c)) {
         char p = board[r][c];
         char upper = std::toupper(static_cast<unsigned char>(p));
         if (upper == 'K') return true;
      }
   }

   return false;
}

void Board::makeMove(int selectedRow, int selectedCol, int row, int col, char& selectedPiece) 
{
   char piece = board[selectedRow][selectedCol];
   board[selectedRow][selectedCol] = '.';
   board[row][col] = selectedPiece;
   pieceSelected = false;
   selectedRow = -1;
   selectedCol = -1;
   selectedPiece = '\0';
   if (getOrderMoves()) { setOrderMoves(false); }
   else { setOrderMoves(true); }

   // 3. Обновляем координаты короля, если это король
   char upper = std::toupper(static_cast<unsigned char>(piece));
   if (upper == 'K') {
      if (std::isupper(static_cast<unsigned char>(piece))) {
         whiteKingRow = row;
         whiteKingCol = col;
      }
      else {
         blackKingRow = row;
         blackKingCol = col;
      }
   }

   // 4. (Позже) обработка рокировки, en passant, превращения пешки
}

const std::array<std::array<char, 8>, 8>& Board::getBoard() const {
   return board;
}

bool Board::isPieceSelected() const {
   return pieceSelected;
}

int Board::getSelectedRow() const {
   return selectedRow;
}

int Board::getSelectedCol() const 
{
   return selectedCol;
}

bool Board::getOrderMoves() const
{
   return orderMoves;
}

void Board::setOrderMoves(bool order)
{
   orderMoves = order;
}
