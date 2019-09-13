CC = g++
CXXFLAGS = -std=c++14 -g -Wall -Werror=vla

all: chess

chess: main.o computer1.o computer2.o computer3.o computer4.o player.o gamestate.o square.o textdisplay.o graphicsdisplay.o window.o king.o queen.o rook.o bishop.o knight.o pawn.o none.o piece.o
	$(CC) $(CXXFLAGS) -o chess main.o computer1.o computer2.o computer3.o computer4.o player.o gamestate.o square.o textdisplay.o graphicsdisplay.o window.o king.o queen.o rook.o bishop.o knight.o pawn.o none.o piece.o message.o move.o -lX11

main.o: main.cc computer1.o computer2.o computer3.o computer4.o player.o gamestate.o square.o graphicsdisplay.o window.o king.o queen.o rook.o bishop.o knight.o pawn.o none.o piece.o message.o move.o
	$(CC) $(CXXFLAGS) -c main.cc

computer1.o: computer1.cc computer1.h enums.h gamestate.o player.o
	$(CC) $(CXXFLAGS) -c computer1.cc

computer2.o: computer2.cc computer2.h enums.h gamestate.o player.o
	$(CC) $(CXXFLAGS) -c computer2.cc

computer3.o: computer3.cc computer3.h enums.h gamestate.o player.o
	$(CC) $(CXXFLAGS) -c computer3.cc

computer4.o: computer4.cc computer4.h enums.h gamestate.o player.o
	$(CC) $(CXXFLAGS) -c computer4.cc

player.o: player.cc player.h enums.h gamestate.o move.o
	$(CC) $(CXXFLAGS) -c player.cc

gamestate.o: gamestate.cc gamestate.h square.o observer.h piece.o king.o queen.o rook.o bishop.o knight.o pawn.o none.o enums.h move.o
	$(CC) $(CXXFLAGS) -c gamestate.cc

square.o: square.cc square.h subject.h observer.h enums.h piece.o message.o info.h none.o move.o queen.o rook.o bishop.o knight.o
	$(CC) $(CXXFLAGS) -c square.cc

textdisplay.o: textdisplay.cc textdisplay.h observer.h subject.h info.h enums.h message.o info.h piece.o
	$(CC) $(CXXFLAGS) -c textdisplay.cc

graphicsdisplay.o: graphicsdisplay.cc graphicsdisplay.h observer.h subject.h info.h enums.h window.o message.o info.h piece.o
	$(CC) $(CXXFLAGS) -c graphicsdisplay.cc

window.o: window.cc window.h
	$(CC) $(CXXFLAGS) -c window.cc

king.o: king.cc king.h enums.h piece.o
	$(CC) $(CXXFLAGS) -c king.cc

queen.o: queen.cc queen.h enums.h piece.o
	$(CC) $(CXXFLAGS) -c queen.cc

rook.o: rook.cc rook.h enums.h piece.o
	$(CC) $(CXXFLAGS) -c rook.cc

bishop.o: bishop.cc bishop.h enums.h piece.o
	$(CC) $(CXXFLAGS) -c bishop.cc

knight.o: knight.cc knight.h enums.h piece.o
	$(CC) $(CXXFLAGS) -c knight.cc

pawn.o: pawn.cc pawn.h enums.h piece.o
	$(CC) $(CXXFLAGS) -c pawn.cc

none.o: none.cc none.h enums.h piece.o
	$(CC) $(CXXFLAGS) -c none.cc

piece.o: piece.cc piece.h enums.h move.o
	$(CC) $(CXXFLAGS) -c piece.cc

message.o: message.cc message.h enums.h move.o
	$(CC) $(CXXFLAGS) -c message.cc

move.o: move.cc move.h
	$(CC) $(CXXFLAGS) -c move.cc

clean: 
	-rm *.o chess
