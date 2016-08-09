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
};å



#endif //PHRASEGRAPH_SENTENCEGRAPH_H
