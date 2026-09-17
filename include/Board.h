#pragma once
#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <vector>
#include <cctype>

class Board
{
private:
   struct Move // откуда ход и куда
   {
      int fromCol;
      int fromRow;
      int toCol;
      int toRow;
   };
   std::array<std::array<char, 8>, 8> board;
   bool pieceSelected;
   int selectedRow;
   int selectedCol;
   char selectedPiece;
   std::vector<Move> legalMoves;
   bool orderMoves = true; // true - ходят белые, иначе чёрные
   short whiteKingRow{ 7 }; // позиции королей
   short whiteKingCol{ 4 };
   short blackKingRow{ 0 };
   short blackKingCol{ 4 };


public:

   Board();       // конструктор вызывает init()
   void init();   // начальная расстановка 
   void handleClick(int row, int col);                // обработка клика (выбор/перемещение)
   std::vector<Move> generatePseudoLegalMoves(int row, int col) const;  // создаёт псевдолегальные ходы
   std::vector<Board::Move> generateLegalMoves(int row, int col) const; // делает предварительный ход, чтобы отсеить нелегальные
   bool isLegalMove(const std::vector<Move>& legalMoves, int selectedRow, int selectedCol, int toRow, int toCol) const; // проверяет наличие легального хода
   bool isMoveLegal(const Move& move, bool isWhite) const;
   bool isSquareAttacked(const char board[8][8], int row, int col, bool byWhite) const;
   void makeMove(int selectedRow, int selectedCol, int row, int col, char& selectedPiece);

   // Геттеры 
   const std::array<std::array<char, 8>, 8>& getBoard() const;
   bool isPieceSelected() const;
   int getSelectedRow() const;
   int getSelectedCol() const; 
   bool getOrderMoves() const;

   // Сеттеры
   void setOrderMoves(bool order);
};

#endif // BOARD_H



