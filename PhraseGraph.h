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


class Phrase{
public:
    int id;   //the id of the phrase
    int head;
    std::vector<int> components;   //the list of dependency nodes

    bool isPredicate;
    bool isArgument;

    std::vector<int> slots;    //list all the slot nodes

    int nargs=0;

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
    void dfs(int start,int previous,int *visited,Phrase* extract_phrase,int* pid);
    void find_head(Phrase* phrase);
    std::string to_string();

};


class Graphs{
public:
    std::vector<PhraseGraph>phrasegraphs;
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
