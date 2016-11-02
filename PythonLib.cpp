//
// Created by wpf on 10/24/16.
//

#include <iostream>
#include <sstream>
#include "Zpar.h"
#include "ZparTree.h"
#include "init.h"

extern bimap word2id;
extern bimap dep2id;

extern "C"  char* get_xy_path(const char* sent){

    static Zpar zp("/home/wpf/zpar-0.7.5/chinese-models");

    std::string sentence= sent;
    std::string aaa=zp.depparser(sentence);

    std::istringstream  iss(aaa);

    char* res = new char[256];


    std::string line;
    std::string lexeme,pos,parent_id,dependency;

    ZparTree *zparTree=new ZparTree();

    int DocId=0;
    int SentenceId = 0;

    word2id.insert({"Unknown",0});
    word2id.insert({"*",1});

    pos2id().insert({"*",0});
    dep2id.insert({"*",0});

    while(getline(iss,line)){

        if(line.empty()){
            zparTree->set_children_array();
            zparTree->idInDocument = DocId;
            zparTree->idInSentence = SentenceId;

            std::string sen_res = DIRT_X_Y(*zparTree);


            delete zparTree;

            zparTree = new ZparTree();
            SentenceId++;

            strcpy(res,sen_res.c_str());
            return res;
        }
        else{
            istringstream is(line);

            is>>lexeme>>pos>>parent_id>>dependency;

            if(word2id.left.find(lexeme)==word2id.left.end()){
                word2id.insert({lexeme,start_wid++});
            }

            if(dep2id.left.find(dependency)==dep2id.left.end()){
                dep2id.insert({dependency,start_did++});
            }

            zparTree->add_node(ZparNode(word2id.left.at(lexeme),pos2id().left.at(pos),stoi(parent_id),dep2id.left.at(dependency),DocId,SentenceId));
        }
    }
}

int main(){
    const char* path = get_xy_path("X 正在研究 Y");
    std::cout<<path<<std::endl;
 }
