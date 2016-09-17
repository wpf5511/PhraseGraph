//
// Created by 王鹏飞 on 16/7/31.
//

#include "SentenceGraph.h"
#include "PhraseGraph.h"

SentenceGraph::SentenceGraph(const PhraseGraph &phraseGraph) {
    this->idInDocument = phraseGraph.idInDocument;
    this->sentenceid = phraseGraph.idInSentence;
    this->senContent = phraseGraph.sen_content;
    this->phrases_num = phraseGraph.pid_to_ident.size();
}

SentenceIdentity::SentenceIdentity(int Docid, int Senid) {
    idInDocument = Docid;
    sentenceId = Senid;
}

SentenceIdentity SentenceGraph::getSenIdent() {

    return  SentenceIdentity(this->idInDocument,this->sentenceid);

}

void Sentences::add_sentence(SentenceGraph senGraph) {

    sentence_map.insert({senGraph.getSenIdent(),senGraph});

}

bool SentenceIdentity::operator<(const SentenceIdentity &other) const{
    if(this->idInDocument!=other.idInDocument){
        return this->idInDocument<other.idInDocument;
    } else{
        return this->sentenceId<other.sentenceId;
    }
}