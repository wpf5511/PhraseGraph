#include <iostream>
#include <fstream>
#include <sstream>
#include "ZparTree.h"
#include "PhraseGraph.h"
#include <cereal/archives/xml.hpp>

using namespace std;

int main() {


    ifstream Zparfile("/Users/wangpf/Downloads/reference-44.txt",ios::in);

    string line;
    string lexeme,pos,parent_id,dependency;

    std::vector<ZparTree> zpars;

    ZparTree *zparTree=new ZparTree();
    while(getline(Zparfile,line)){
        if(line.empty()){
            zparTree->set_children_array();
            zpars.push_back(*zparTree);
            delete zparTree;
            zparTree = new ZparTree();
        }
        else{
            istringstream is(line);
            is>>lexeme>>pos>>parent_id>>dependency;

            zparTree->add_node(ZparNode(lexeme,pos,stoi(parent_id),dependency));
        }
    }

    cout<<zpars.size()<<endl;

    ifstream verb_file("/Users/wangpf/ClionProjects/Infer-from-tree/普通动词表");
    set<std::string>verb_dict;
    string word;
    while(getline(verb_file,word)){
        verb_dict.insert(word);
    }
    verb_file.close();

    Graphs graphs;

    //for(int i=0;i<zpars.size();i++){

        zpars[0].preprocessing(verb_dict);

        cout<<"hello"<<endl;

        PhraseGraph graph(zpars[0]);

        graph.extract_Template();

       // cout<<graph.phrases.size()<<endl;

       // graphs.phrasegraphs.push_back(graph);
        cout<<graph.to_string()<<endl;

    //}

     /*std::ofstream os("/Users/wangpf/graphs",ios::binary);

     cereal::BinaryOutputArchive archive( os );

     archive(graphs);

     os.close();



    Graphs gtest ;
    ifstream ifs("/Users/wangpf/graphs", std::ios::binary);
    cereal::BinaryInputArchive iarchive(ifs);
    iarchive(gtest);

    cout<<"hello"<<std::endl;*/

    return 0;
}