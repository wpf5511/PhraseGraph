//
// Created by wpf on 9/14/16.
//

#include <iostream>
#include "ZparTree.h"

#ifndef PHRASEGRAPH_PHRASETRANSFORM_H
#define PHRASEGRAPH_PHRASETRANSFORM_H


class PhraseTransform {

public:

    std::string  phrasestring;

    ZparTree   phrasetree;

    static  char FIELD_SEPARATOR;

    static  char OPEN_BRACE;

    static  char CLOSE_BRACE;

    bool operator<(const PhraseTransform&other) const;

    PhraseTransform(const ZparTree ztree);

    std::string  getStringRepresent();

    void  setStringRepresent();


    ZparTree  getTreeRepresent();

    std::string  createStringRepresent(int id);
};


#endif //PHRASEGRAPH_PHRASETRANSFORM_H
