//
// Created by wpf on 9/14/16.
//

#include <fstream>
#include <sstream>
#include "PhraseTransform.h"
using namespace std;
int main(){

    std::ifstream Zparfile("/home/wpf/test-phrase-tree.txt",std::ios::in);

    string line;

    string lexeme,pos,parent_id,dependency;

    ZparTree *zparTree=new ZparTree();

    int SentenceId = 1;

    vector<ZparTree>zpars;

    while(getline(Zparfile,line)){
        if(line.empty()){
            zparTree->set_children_array();
            zparTree->idInDocument = 1;
            zparTree->idInSentence = 1;
            zpars.push_back(*zparTree);  // store or not store;
            delete zparTree;
            zparTree = new ZparTree();
        }
        else{
            istringstream is(line);

            is>>lexeme>>pos>>parent_id>>dependency;

            zparTree->add_node(ZparNode(lexeme,pos,stoi(parent_id),dependency,1,SentenceId));
        }
    }

    Zparfile.close();

    PhraseTransform  pt = PhraseTransform(zpars[0]);

    std::string res = pt.getStringRepresent();

    cout<<res<<endl;
    return 0;
}