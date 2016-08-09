//
// Created by 王鹏飞 on 16/7/18.
//

#ifndef PHRASEGRAPH_PHRASEGRAPH_H
#define PHRASEGRAPH_PHRASEGRAPH_H

#include "ZparTree.h"
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

class PhraseIdentify{

    int  idInDocument;
    int  idInSentence;
    int  PhraseId;

};


class Phrase{
public:

    int idInDocument;
    int idInSentence;

    int id;   //the id of the phrase
    int head;
    int nargs=0;

    std::vector<int> components;   //the list of dependency nodes
    std::string content;

    bool isPredicate;
    bool isArgument;
    bool isCoo;


    std::vector<int> slots;    //list all the slot nodes

    std::vector<int> Coos;

    PhraseIdentify coreferPhrase;//指向的Phrase




    Phrase()= default;

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
    ZparTree ztree;
    std::map<int,Phrase> phrases;
    std::map<int,int> node_to_phrase;

    static std::map<std::string,bool> hashPOSOfNoun;
    static std::map<std::string,bool> hashPOSOfVerb;

    static std::map<std::string,bool> hashPOSOfNPHead;
    static std::map<std::string,bool> hashPOSOfNOModifier;

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(ztree,phrases,node_to_phrase);
    }


public:
    PhraseGraph(ZparTree ztree);
    PhraseGraph()= default;
    void extract_Template();
    bool is_end(int nodeid);
    bool is_verb(int nodeid);
    bool is_noun(int nodeid);
    bool CanBeModifier(int nodeid);
    bool CanBeHead(int nodeid);

    void dfsnn(int start,int previous,int *visited,Phrase* extract_phrase,int* pid,std::map<int,bool>&hashIncluded,std::vector<int>&vvec);


    void dfscm(int start,int previous,int *visited,Phrase* extract_phrase,int* pid,std::map<int,bool>&hashIncluded,std::vector<int>&vvec);


    void dfsvv(int start,int previous,int *visited,Phrase* extract_phrase,int* pid,std::map<int,bool>hashIncluded);

    void find_head(Phrase* phrase);
    std::string to_string();

};


class Graphs{
public:
    std::vector<PhraseGraph>phrasegraphs;
    std::map<PhraseIdentify,Phrase> phrase_map;
public:


    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(phrasegraphs);
    }


    //void load_Graphs(std::string graphfile);

    //void save_Graphs(std::string graphfile);
};


#endif //PHRASEGRAPH_PHRASEGRAPH_H
