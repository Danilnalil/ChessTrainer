#pragma once
#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <vector>
#include <cctype>


class Board
{

public:
   // тип шахматного хода
   enum class MoveType
   {
      Normal, 
      CastleKingSide,
      CastleQueenSide,
      EnPassant,
      Promotion
   };
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
   // конструктор вызывает init()
   Board();       
   // начальная расстановка 
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

   // Геттеры 
   const std::array<std::array<char, 8>, 8>& getBoard() const;
   bool isPieceSelected() const;
   int getSelectedRow() const;
   int getSelectedCol() const; 
   bool getOrderMoves() const;

   // Сеттеры
   void setOrderMoves(bool order);

private:
   // откуда ход и куда

   std::array<std::array<char, 8>, 8> board;
   bool pieceSelected;
   int selectedRow;
   int selectedCol;
   char selectedPiece;
   std::vector<Move> legalMoves;
   bool whiteToMove = true; // true - ходят белые, иначе чёрные
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
   

};

#endif // BOARD_H



