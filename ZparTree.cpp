//
// Created by 王鹏飞 on 16/6/23.
//

#include "ZparTree.h"
#include <sstream>
#include <iostream>

//ZparNode method

int ZparNode::get_parent() {
    return this->parent_id;
}

int ZparNode::get_id(){
    return this->id;
}

ZparNode::ZparNode(std::string lexeme, std::string pos, int parent_id, std::string dependency,
                   bool isVirtual,bool isSlot,int link,int level) {

    this->lexeme = lexeme;
    this->pos = pos;
    this->parent_id =parent_id;
    this->dependency = dependency;
    this->sentense_position = id;

     this->isVirtual= isVirtual;

     this->isSlot= isSlot;
     this->link= link;

    this->level= level;

}

ZparNode::ZparNode(const ZparNode &node) {

    this->id = node.id;
    this->lexeme = node.lexeme;
    this->pos = node.pos;
    this->parent_id = node.parent_id;
    this->dependency = node.dependency;
    this->sentense_position = node.sentense_position;

    this->isVirtual = node.isVirtual;
    this->isSlot = node.isSlot;
    this->link =node.link;
    this->level = node.level;
}



std::string ZparNode::get_pos() {
    return this->pos;
}

std::string ZparNode::get_dependency() {
    return this->dependency;
}

std::string ZparNode::get_lexeme() {
    return this->lexeme;
}




//ZparTree method

void ZparTree::add_node(ZparNode node2) {

    int j = this->nodes.size();

    node2.id = j;
    node2.sentense_position=j;


    if(node2.parent_id==-1){
        root_id=j;
    }

    this->nodes.push_back(node2);

}

void ZparTree::add_node(ZparNode node2,int sentence_position) {

    int j = this->nodes.size();

    node2.id = j;

    node2.sentense_position=sentence_position;

    node2.isVirtual = true;

    if(node2.parent_id==-1){
        root_id=j;
    }

    this->nodes.push_back(node2);

}





void ZparTree::set_children_array() {

    children_array.clear();
    for(int i=0;i<nodes.size();i++){
        if(nodes[i].get_parent()!=-1){
            children_array[nodes[i].get_parent()].push_back(i);
        }
    }

}

std::vector<int> ZparTree::get_children(int id) {

    return children_array[id];  //at 处理单独成句的会出现异常

}


ZparNode& ZparTree::get_Node(int id) {
    return nodes[id];
}

std::string ZparTree::to_sentence() {
    std::ostringstream out_sen;

    for(int i=0;i<nodes.size();i++){
        out_sen<<nodes[i].get_lexeme()<<" ";
    }
    return out_sen.str();
}


void ZparTree::preprocessing(std::set<std::string> verb_dict) {

    for(int i=0;i<this->nodes.size();i++){
        auto & znode= get_Node(i);
        if(znode.pos=="NN"){

            std::string word = znode.lexeme;
            if(verb_dict.find(word)!=verb_dict.end()){
                znode.pos = "VV";
            }
        }
        if(znode.pos=="DEC"){
            int parent_id = znode.parent_id;
            ZparNode dec_parent = get_Node(parent_id);
            std::string dec_parent_pos = dec_parent.pos;
            if(dec_parent_pos=="VA"||dec_parent_pos=="VC"||dec_parent_pos=="VE"||dec_parent_pos=="VV"){

                znode.parent_id =nodes.size();
                add_node(ZparNode("","NN",parent_id,znode.dependency),i+1);

                auto &zznode = get_Node(i);

                zznode.dependency="NMOD";
            }
        }
    }
    set_children_array();
    set_level(root_id,1);
}

void ZparTree::set_level(int id,int level){

    auto &node = get_Node(id);
    node.level = level;

    std::vector<int> children_id=get_children(id);

    for(int i=0;i<children_id.size();i++){
        set_level(children_id[i],level+1);
    }

}
