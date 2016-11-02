#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <thread>
#include "ZparTree.h"
#include "PhraseTransform.h"
#include "init.h"
#include <cereal/archives/xml.hpp>
#include <dirent.h>
#include <time.h>
using namespace std;




const int THRED_NUM = 15;

void ReadDict(std::string path,set<std::string>&verb_dict){

    ifstream verb_file(path);

    string word;

    while(getline(verb_file,word)){
        verb_dict.insert(word);
    }

    verb_file.close();

}


void FindTreeStart(ifstream& in){

    std::string line;
    while(getline(in,line)){
        if(line=="\n")
            break;
    }
}

void extractNEP(std::vector<Token> tokens, std::map<SentenceIdentity, NEPMAP> &senNEP){

    std::string phraseNer = "newPhrase";
    std::string phraseText = "";
    for(int i=0;i<tokens.size();i++){
        if (tokens[i].ner != "O" && (tokens[i].ner == phraseNer || phraseNer == "newPhrase")){
            phraseText  = phraseText + "_" + tokens[i].word;
            phraseNer = tokens[i].ner;
        }
        else{
            if (phraseText.length() != 0){
                SentenceIdentity Sident = SentenceIdentity(tokens[i-1].Documentid,tokens[i-1].sentenceid);

                senNEP[Sident][tokens[i].tokenid- 1] = {tokens[i-1].word,phraseText.substr(1,-1),tokens[i-1].ner};

                phraseText = "";
                phraseNer = "newPhrase";
            }
        }
    }


}

void BuildGraph(vector<ZparTree>&zpars,Graphs& graphs,const set<std::string>&verb_dict, const std::vector<Token>& nerTokens,const std::vector<Corefer>& corefers,Sentences& sens){
 
    clock_t build_start=clock();
    cout<<"build start"<<endl;
   // std::map<SentenceIdentity, NEPMAP> senNEP;  //所有的句子nep

   // extractNEP(nerTokens,senNEP);   //可在所有句子，可在一个文档执行

    for(int i=0;i<zpars.size();i++){
         
         if(i%5000==0)
		 cout<<i<<endl;
         zpars[i].preprocessing(verb_dict);

         PhraseGraph *graph=new PhraseGraph(zpars[i]);

         graph->extract_Phrases(graphs.phrase_map);


      // auto SIdent = graph.getSenIdent();

      //  auto CurNEP = senNEP[SIdent];   // 单个句子的NEP

      //  graph.addNEtoPhrase(CurNEP,graphs.phrase_map);

      // SentenceGraph sentenceGraph(graph);

     //   sens.add_sentence(sentenceGraph);

        graphs.phrasegraphs.push_back(graph);

    }

    // graphs.addCorefertoPhrase(sens.sentence_map,graphs.phrase_map,corefers);
    clock_t build_end = clock();
    cout<<"build time:"<<(build_end-build_start)/1000000<<endl;

}



ZparTree getPhraseTree(Phrase*  phrase,ZparTree sentenceTree){

    ZparTree resTree;

    vector<int> components = phrase->components;

    for(int i=0;i<components.size();i++){

        auto &node=sentenceTree.get_Node(components[i]);

        resTree.add_node(node, true);

    }

    vector<int> slots = phrase->slots;

    for(int j=0;j<slots.size();j++){

        auto &slot_node = sentenceTree.get_Node(slots[j]);

        resTree.add_slot(slot_node, true);
    }

    resTree.set_children_array();

    return resTree;
}

void savePhraseIdx(map<std::string,int> np_to_id,std::string filepath){
    ofstream out(filepath,ios::out);

    for(auto iter = np_to_id.begin();iter!=np_to_id.end();iter++){
        out<<iter->first<<"\t"<<iter->second<<endl;
    }

    out.close();
}

void savePatternIdx(std::map<int,std::map<int,std::map<int,int>>> vp_to_nps,std::string filepath){
    ofstream out(filepath,ios::out);

    for(auto iter = vp_to_nps.begin();iter!=vp_to_nps.end();iter++){
        int vpId = iter->first;
        out<<vpId<<":";
        for(auto nps_it = vp_to_nps[vpId].begin();nps_it!=vp_to_nps[vpId].end();nps_it++){
            int np1Id = nps_it->first;
            for(auto np2_it = vp_to_nps[vpId][np1Id].begin();np2_it!=vp_to_nps[vpId][np1Id].end();np2_it++){
                int np2Id = np2_it->first;
                int count = np2_it->second;
                out<<np1Id<<","<<np2Id<<","<<count;
                out<<"|";
            }
        }

        out<<"\n";
    }
    out.close();
}


int main() {

    const char* input_zpared="/home/wpf/input-zpared";
    const char* input_stanford = "/home/wpf/Downloads/input-stanford";
    std::thread threads[THRED_NUM];

   DIR           *d;
   struct dirent *dir;
   d = opendir(input_zpared);
   if (d)
   {
       set<std::string>verb_dict;
       ReadDict("/home/wpf/verb_dict.txt",verb_dict);

       int DocId=0;
       std::vector<ZparTree> zpars;

       Graphs graphs;

       std::vector<Token> nerTokens;

       std::vector<Corefer> corefers;

       Sentences sentences;

       std::ofstream out("/home/wpf/cn/triple_cn",ios::out);

       while ((dir = readdir(d)) != NULL)
       {
           if (dir->d_name[0] == '.')
               continue;
           std::string  filename,xmlfile;
           filename = filename+input_zpared+"/"+dir->d_name;

           std::string ss = dir->d_name;

           xmlfile = xmlfile+input_stanford+"/"+ss+".xml";

           ReadFromZpar(filename,DocId,out);

          // CommonTool::getInfofromStanford(xmlfile,nerTokens,corefers,DocId);

           DocId++;

       }
       cout<<zpars.size()<<endl;

       //BuildGraph(zpars,graphs,verb_dict,nerTokens,corefers,sentences);   // 可以提出去



       //抽出所有的np先


       //遍历所有的np对
       closedir(d);
   }

    return 0;
}
