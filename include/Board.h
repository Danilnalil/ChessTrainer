#pragma once
#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <vector>
#include <cctype>
#include <cstdlib> 
/*
 * ============================================================================
 *  КЛАСС Board — модель шахматной доски и правил игры.
 * ----------------------------------------------------------------------------
 *  Отвечает за:
 *    • хранение текущей расстановки фигур (8x8);
 *    • генерацию ходов (псевдолегальных и легальных);
 *    • проверку шаха и атакованных клеток;
 *    • применение ходов (makeMove) и обновление состояния партии.
 *
 *  НЕ знает ничего про SFML/графику — это чистая логика.
 *  Взаимодействие с UI идёт через handleClick() и геттеры.
 *
 *  Система координат:
 *    board[row][col], row 0 — верх (чёрные), row 7 — низ (белые).
 *    col 0 — левый край (линия "a"), col 7 — правый (линия "h").
 *
 *  Нотация фигур (символы в клетках):
 *    'K','Q','R','B','N','P' — белые,
 *    'k','q','r','b','n','p' — чёрные,
 *    '.'                     — пустая клетка.
 * ============================================================================
 */

class Board
{

public:
 /* ------------------------------------------------------------------------
 *  MoveType — тип шахматного хода.
 *  Нужен, чтобы makeMove() знал, как обрабатывать особые случаи:
 *  рокировку, взятие на проходе, превращение пешки.
 * --------------------------------------------------------------------- 
 */

   enum class MoveType
   {
      Normal,           // обычный ход (в т.ч. взятие)
      CastleKingSide,   // короткая рокировка O-O
      CastleQueenSide,  // длинная рокировка O-O-O
      EnPassant,        // взятие на проходе (пока не реализовано)
      Promotion         // превращение пешки (пока не реализовано)
   };

/* ------------------------------------------------------------------------
 *  GameState — состояние партии.
 *  Обновляется после каждого хода в makeMove():
 *    Playing    — игра идёт;
 *    WhiteWins  — мат, победа белых (ход чёрным, их король под шахом, ходов нет);
 *    BlackWins  — мат, победа чёрных;
 *    Draw       — пат (ход есть, но легальных ходов нет, король НЕ под шахом).
 * --------------------------------------------------------------------- */
   enum class GameState
   {
      Playing,
      WhiteWins,
      BlackWins,
      Draw
   };

 /* ------------------------------------------------------------------------
 *  Move — структура хода.
 *  ВНИМАНИЕ: порядок полей — СНАЧАЛА col, ПОТОМ row.
 *  Это важно, т.к. в коде часто пишут { fromCol, fromRow, toCol, toRow }.
 * --------------------------------------------------------------------- 
 */
   struct Move
   {
      int fromCol;
      int fromRow;
      int toCol;
      int toRow;
      // по умолчанию ход - обычный
      MoveType type = MoveType::Normal;
      // используем для превращения пешки ('\0' - нет превращения)
      char promotionPiece = '\0';
   };
   // Конструктор: обнуляет состояние выбора и вызывает init() для расстановки.
   Board();       
   // Расставляет фигуры в начальную позицию и сбрасывает всё состояние партии
   // (флаги рокировок, координаты королей, очередь хода, вектор ходов).
   void init();
   // обработка клика (выбор/перемещение)
   void handleClick(int row, int col);   
   // создаёт псевдолегальные ходы
   std::vector<Move> generatePseudoLegalMoves(int row, int col) const;
   // делает предварительный ход, чтобы отсеить нелегальные
   std::vector<Board::Move> generateLegalMoves(int row, int col) const;
   // функция ищет конкретный легальный ход
   // если найден - указатель, если нет - nullptr
   const Move* findLegalMove (const std::vector<Move>& legalMoves, int selectedRow, int selectedCol, int toRow, int toCol) const;
   bool isMoveLegal(const Move& move, bool isWhite) const;
   bool isSquareAttacked(const std::array<std::array<char, 8>, 8>& board, int row, int col, bool byWhite) const;
   void makeMove(const Move& move);
   // true — сейчас нужно показать игроку попап выбора фигуры
   bool isWaitingForPromotion() const; 

   void confirmPromotion(char piece);
   // Состояние партии после последнего хода (Playing / WhiteWins / BlackWins / Draw)
   GameState getGameState() const;
   // true — партия окончена (мат или пат): UI показывает плашку с результатом
   bool isGameOver() const;
   // Геттеры 
   const std::array<std::array<char, 8>, 8>& getBoard() const;
   bool isPieceSelected() const;
   int getSelectedRow() const;
   int getSelectedCol() const; 
   bool getOrderMoves() const;
   int getPromotionRow() const;
   int getPromotionCol() const;
   // Сеттеры
   void setOrderMoves(bool order);

private:
   // откуда ход и куда

   std::array<std::array<char, 8>, 8> board;
   bool pieceSelected;
   int selectedRow;
   int selectedCol;
   char selectedPiece;

   int enPassantRow = -1; // клетка для взятия на проходе
   int enPassantCol = -1;

   bool awaitingPromotion = false; // состояние превращения (true — пешка дошла до последней линии, ждём выбора фигуры игроком)
   Move pendingPromotion; // Сохранённый ход, который нужно выполнить после выбора фигуры.

   std::vector<Move> legalMoves;
   bool whiteToMove = true; // true - ходят белые, иначе чёрные
   GameState gameState = GameState::Playing; // текущее состояние партии (мат/пат обновляются после каждого хода)

   short whiteKingRow{ 7 }; // позиции королей
   short whiteKingCol{ 4 };
   short blackKingRow{ 0 };
   short blackKingCol{ 4 };

   //===========================================
   // ПРАВА НА РОКИРОВКУ
   //
   // 
   // Белые ещё имеют право на короткую рокировку O-O.
   bool whiteCanCastleKingSide = true;

   // Белые ещё имеют право на длинную рокировку O-O-O.
   bool whiteCanCastleQueenSide = true;

   // То же самое для чёрных.
   bool blackCanCastleKingSide = true;
   bool blackCanCastleQueenSide = true;
   //=============================================
   // ПРОВЕРКА ВОЗМОЖНОСТИ РОКИРОВКИ
   // 

   // Проверяет короткую рокировку.
   // white == true  -> проверяем белых.
   // white == false -> проверяем чёрных.
   bool canCastleKingSide(const std::array<std::array<char, 8>, 8>& board, bool white) const;

   // Проверяет длинную рокировку.
   // white == true  -> проверяем белых.
   // white == false -> проверяем чёрных.
   bool canCastleQueenSide(const std::array<std::array<char, 8>, 8>& board, bool white) const;

   // Есть ли у стороны нужного цвета хотя бы один легальный ход.
   // Используется для детекции мата/пата после каждого хода.
   bool hasAnyLegalMove(bool forWhite) const;

   // Атакован ли король стороны нужного цвета на текущей доске (шах).
   bool isInCheck(bool byWhite) const;

   // Определяет состояние партии для стороны, которой сейчас предстоит ход:
   // мат (победа противоположной стороны), пат (ничья) или игра продолжается.
   void updateGameState();
   

};

#endif // BOARD_H



