for s in ['King', 'Queen', 'Rook', 'Bishop', 'Knight', 'Pawn', 'None']:
    doc = open('piece.h').read().split('\n')
    doc[0] = doc[0].replace('PIECE', s.upper())
    doc[1] = doc[1].replace('PIECE', s.upper())
    decl = 7
    doc[decl] = doc[decl].replace('Piece', s)
    ind = doc[decl].find('{')
    doc[decl] = doc[decl][:ind] + ': public Piece ' + doc[decl][ind:]
    doc = doc[:(decl + 1)] + doc[(decl + 5):]
    doc = doc[:(decl - 1)] + ['#include "piece.h"'] + doc[(decl - 1):]
    if s != 'Pawn':
        doc = [l for l in doc if 'isPotentialCapture' not in l]
    if s != 'Pawn' and s != 'King':
        doc = [l for l in doc if 'classifyMove' not in l]
    doc = [l for l in doc if '~Piece' not in l]
    doc = [l for l in doc if 'operator==' not in l]
    for i, l in enumerate(doc):
        if 'Piece(PieceType type' in l:
            doc[i] = doc[i].replace('PieceType type, ', '')
            doc[i] = doc[i].replace('Piece', s)
            if s == 'None':
                doc[i] = doc[i].replace('Colour colour, ', '')
        if 'virtual' in l:
            doc[i] = doc[i].replace('virtual ', '')
            doc[i] = doc[i].replace(' = 0', '')
            doc[i] = doc[i].replace(';', ' override;')
    open(s.lower() + '.h', 'w').write('\n'.join(doc))
