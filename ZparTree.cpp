//
// Created by 王鹏飞 on 16/6/23.
//

#include "ZparTree.h"
#include <sstream>
#include <iostream>
#include <algorithm>

//ZparNode method

ZparNode::ZparNode(std::string lexeme, std::string pos, int parent_id, std::string dependency,int idInDocument,int idInSentence,
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

     this->idInDocument = idInDocument;
     this->idInSentence = idInSentence;
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
    this->idInDocument = node.idInDocument;
    this->idInSentence = node.idInSentence;
}


//ZparTree method
ZparTree::ZparTree(const ZparTree& cztree) {

    this->idInDocument=cztree.idInDocument;

    this->idInSentence=cztree.idInSentence;

    this->nodes = cztree.nodes;

    this->children_array=cztree.children_array;
    this->root_id=cztree.root_id;
    this->min_level=cztree.min_level;

}



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

    if(node2.parent_id==-1){
        root_id=j;
    }

    this->nodes.push_back(node2);

}


void ZparTree::add_node(ZparNode node3,bool has_id){

    if(has_id){

        if(node3.level<this->min_level){
            min_level = node3.level;
            root_id=node3.id;
        }

        this->nodes.push_back(node3);

    }

}


void ZparTree::add_slot(ZparNode node3,bool has_id){

    if(has_id){

        this->nodes.push_back(node3);

    }

}





void ZparTree::set_children_array() {

    children_array.clear();
    for(int i=0;i<nodes.size();i++){
        if(nodes[i].parent_id!=-1){
            children_array[nodes[i].parent_id].push_back(nodes[i].id);
        }
    }


}

std::vector<int> ZparTree::get_children(int id) {

    return children_array[id];  //at 处理单独成句的会出现异常

}


ZparNode& ZparTree::get_Node(int nodeid) {

    //按照node在句子中的顺序排序
    //std::sort(nodes.begin(),nodes.end());

    for(int i=0;i<nodes.size();i++){
        if(nodes[i].id==nodeid){
            return nodes[i];
        }
    }
}

std::string ZparTree::to_sentence() {
    std::ostringstream out_sen;

    for(int i=0;i<nodes.size();i++){
        out_sen<<nodes[i].lexeme<<" ";
    }
    return out_sen.str();
}


void ZparTree::preprocessing(std::set<std::string> verb_dict) {

    for(int i=0;i<this->nodes.size();i++){
        auto & znode= get_Node(nodes[i].id);   // before process id is i
        if(znode.pos=="NN"){

            std::string word = znode.lexeme;
            if(verb_dict.find(word)!=verb_dict.end()){
                znode.pos = "VV";
            }
        }
        if(znode.pos=="DEC") {
            int parent_id = znode.parent_id;
            if(parent_id!=-1){
                    ZparNode dec_parent = get_Node(parent_id);
                    std::string dec_parent_pos = dec_parent.pos;
                    if (dec_parent_pos == "VA" || dec_parent_pos == "VC" || dec_parent_pos == "VE" || dec_parent_pos == "VV") {

                        znode.parent_id = nodes.size();

                        int DocId = znode.idInDocument;
                        int SentenceId = znode.idInSentence;
                        add_node(ZparNode("Unknown", "NN", parent_id, znode.dependency, DocId, SentenceId),
                                 znode.sentense_position + 1);

                        znode.dependency = "NMOD";
                    }
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

std::vector<int> ZparTree::getPathFromRoot(int nodeid){

    auto node =get_Node(nodeid);
    std::vector<int>path;

    while(node.parent_id!=-1){
        node = get_Node(node.parent_id);
        path.push_back(node.id);
    }
    std::reverse(path.begin(),path.end());
    return  path;
}

int  ZparTree::getLca(int id1,int id2){

    std::vector<int> path1 = getPathFromRoot(id1);
    std::vector<int> path2 = getPathFromRoot(id2);

    int min_len = std::min(path1.size(),path2.size());

    if(min_len==0){
        int lca_idx = 0;
        if(path1.size()>0){
            return path1[lca_idx];
        } else{
            return path2[lca_idx];
        }
    }

    int i;
    for(i=0;i<min_len;i++){
        if(path1[i]!=path2[i])
            break;
    }

    if(path1[i]==path2[i]){
        int lca_idx = i;
        return  path1[lca_idx];
    } else{
        int lca_idx = i-1;
        return path1[lca_idx];
    }
}

