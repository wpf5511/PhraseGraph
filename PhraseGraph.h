//
// Created by 王鹏飞 on 16/7/18.
//

#ifndef PHRASEGRAPH_PHRASEGRAPH_H
#define PHRASEGRAPH_PHRASEGRAPH_H

#include "ZparTree.h"
#include "CommonTool.h"
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include "SentenceGraph.h"


bool PosComp(int nodeid1,int nodeid2,ZparTree ztree);

class PhraseIdentity{

public:
    int  idInDocument;
    int  idInSentence;
    int  PhraseId;

    PhraseIdentity(int DocId,int SenId,int PhId){
        idInDocument = DocId;
        idInSentence = SenId;
        PhraseId = PhId;
    }

    PhraseIdentity()= default;

    bool operator<(const PhraseIdentity&other) const{
        if(this->idInDocument!=other.idInDocument){
            return this->idInDocument<other.idInDocument;
        } else if(this->idInSentence!=other.idInSentence){
            return this->idInSentence<other.idInSentence;
        } else{
            return this->PhraseId<other.PhraseId;
        }
    }

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(idInDocument,idInSentence,PhraseId);
    }

};


class Phrase{
public:

    int idInDocument;
    int idInSentence;

    int id;   //the id of the phrase
    int head;
    int nargs=0;

    std::vector<int> components;   //the list of dependency nodes
    std::string content;          //content of the phrase

    bool isPredicate;
    bool isArgument;
    bool isRepresent;
    bool isCoo;
    bool isNE;

    std::string entityType;

    std::vector<int> slots;    //list all the slot nodes

    std::vector<int> Coos;

    PhraseIdentity coreferPhrase;//指向的Phrase

    Phrase()= default;

    bool operator< (const Phrase& other)const{
        return this->head<other.head;
    };

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(id,head,components,isPredicate,isArgument,slots,nargs);
    }


    Phrase(int phrase_id){
        id = phrase_id;
        isPredicate = false;
        isArgument = false;
    }

    void add_component(int compid){
        components.push_back(compid);
    }

    void add_slot(int slot_id){
        slots.push_back(slot_id);
        if(isPredicate){
            nargs+=1;
        }
    }
};



class PhraseGraph {
public:
    ZparTree &ztree;
    int idInDocument;
    int idInSentence;
   // std::map<int,Phrase> phrases;//value type should change to id.  former structure

    std::string sen_content;

    std::map<int,PhraseIdentity> pid_to_ident;

    std::map<int,int> node_to_phrase;

    static std::map<int,bool> hashPOSOfNoun;
    static std::map<int,bool> hashPOSOfVerb;

    static std::map<int,bool> hashPOSOfNPHead;
    static std::map<int,bool> hashPOSOfNOModifier;

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(ztree,node_to_phrase);
    }


public:
    PhraseGraph(ZparTree& zparTree);
    PhraseGraph()= default;
    void extract_Phrases(std::map<PhraseIdentity,Phrase*> &phrase_map);
    bool is_end(int nodeid);
    bool is_verb(int nodeid);
    bool is_noun(int nodeid);
    bool CanBeModifier(int nodeid);
    bool CanBeHead(int nodeid);

    void dfsnn(int start,int previous,Phrase* extract_phrase,int* pid,std::map<int,bool>&hashIncluded,std::vector<int>&vvec,std::map<PhraseIdentity,Phrase*> &phrase_map);


    void dfscm(int start,int previous,Phrase* extract_phrase,int* pid,std::map<int,bool>&hashIncluded,std::vector<int>&vvec,std::map<PhraseIdentity,Phrase*> &phrase_map);


    void dfsvv(int start,int previous,int *visited,Phrase* extract_phrase,int* pid,std::map<int,bool>hashIncluded,std::map<PhraseIdentity,Phrase*> &phrase_map);

    void find_head(Phrase* phrase);

    void set_content(std::map<PhraseIdentity,Phrase*> &phrase_map);

    std::string to_string(const std::map<PhraseIdentity,Phrase> &phrase_map);

    void addNEtoPhrase(NEPMAP neps,std::map<PhraseIdentity,Phrase> &phrase_map);


    PhraseIdentity getPhraseIdent(int phrase_id){

        return PhraseIdentity(idInDocument,idInSentence,phrase_id);

    }

    SentenceIdentity getSenIdent(){
        return  SentenceIdentity(idInDocument,idInSentence);
    }



};


class Graphs{
public:
    std::vector<PhraseGraph*>phrasegraphs;
    std::map<PhraseIdentity,Phrase*> phrase_map;


    void addCorefertoPhrase(std::map<SentenceIdentity,SentenceGraph> sentence_map,
                            std::map<PhraseIdentity,Phrase> &phrase_map,std::vector<Corefer> corefers);





};


#endif //PHRASEGRAPH_PHRASEGRAPH_H
