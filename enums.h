#ifndef ENUMS_H
#define ENUMS_H
enum class PieceType { King, Queen, Rook, Bishop, Knight, Pawn, NoneType };
enum class MessageType { PieceChanged, PieceMove, PawnCapture, EnPassantRelay, 
                       CastleKing, CastleRookRelay, CastleRookReply, 
                       CheckRelay, CheckReply };
enum class Colour { Black, White, NoColour };
enum class Direction { NW, N, NE, W, E, SW, S, SE, 
                       L1, L2, L3, L4, L5, L6, L7, L8 };
enum class MoveResult { Checkmate, Stalemate, Check, Other, Error };
enum class Status { Error, Continue, Success, Promote, CheckEnPassant, 
                    CheckCheck };
#endif
