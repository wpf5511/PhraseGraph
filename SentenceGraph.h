//
// Created by 王鹏飞 on 16/7/31.
//


#ifndef PHRASEGRAPH_SENTENCEGRAPH_H
#define PHRASEGRAPH_SENTENCEGRAPH_H

#include <string>
#include <map>

class PhraseGraph;


struct SentenceIdentity{
    int  idInDocument;
    int  sentenceId;

    SentenceIdentity(int Docid,int Senid);

    bool operator<(const SentenceIdentity& other) const ;

};



class SentenceGraph {
public:
    int idInDocument;
    int sentenceid;

    std::string senContent;

    int phrases_num;

    SentenceGraph(const PhraseGraph &phraseGraph);

    SentenceIdentity getSenIdent();
};



class Sentences{
public:
    std::map<SentenceIdentity,SentenceGraph> sentence_map;

    void add_sentence(SentenceGraph senGraph);


};



#endif //PHRASEGRAPH_SENTENCEGRAPH_H
