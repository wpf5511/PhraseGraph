//
// Created by 王鹏飞 on 16/7/31.
//

#ifndef PHRASEGRAPH_SENTENCEGRAPH_H
#define PHRASEGRAPH_SENTENCEGRAPH_H


class SentenceGraph {
    int idInDocument;
    int sentenceid;
    std::string senContent;

    std::vector<int>Phrases;
};

struct SentenceIdentify{
    int  idInDocument;
    int  sentenceId;
    SentenceIdentify(int Docid,int Senid){
        idInDocument = Docid;
        sentenceId = Senid;
    }
};

class Sentences{
public:
    std::map<SentenceIdentify,SentenceGraph> sentence_map;
};



#endif //PHRASEGRAPH_SENTENCEGRAPH_H
