#include <iostream>
#include <fstream>
#include <sstream>
#include "ZparTree.h"
#include "PhraseGraph.h"
#include "PhraseTransform.h"
#include <cereal/archives/xml.hpp>
#include <dirent.h>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;



void ReadDict(std::string path,set<std::string>&verb_dict){

    ifstream verb_file(path);

    string word;

    while(getline(verb_file,word)){
        verb_dict.insert(word);
    }

    verb_file.close();

}
void ReadFromZpar(std::string path,int DocId,vector<ZparTree>&zpars){


    ifstream Zparfile(path,ios::in);

    string line;
    string lexeme,pos,parent_id,dependency;

    ZparTree *zparTree=new ZparTree();

    int SentenceId = 0;

    while(getline(Zparfile,line)){
        if(line.empty()){
            zparTree->set_children_array();
            zparTree->idInDocument = DocId;
            zparTree->idInSentence = SentenceId;
            zpars.push_back(*zparTree);  // store or not store;
            delete zparTree;
            zparTree = new ZparTree();
            SentenceId++;
        }
        else{
            istringstream is(line);

            is>>lexeme>>pos>>parent_id>>dependency;

            zparTree->add_node(ZparNode(lexeme,pos,stoi(parent_id),dependency,DocId,SentenceId));
        }
    }

    Zparfile.close();
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

    std::map<SentenceIdentity, NEPMAP> senNEP;  //所有的句子nep

    extractNEP(nerTokens,senNEP);   //可在所有句子，可在一个文档执行

    for(int i=0;i<zpars.size();i++){

         zpars[i].preprocessing(verb_dict);

        PhraseGraph graph(zpars[i]);

        graph.extract_Phrases(graphs.phrase_map);


        auto SIdent = graph.getSenIdent();

        auto CurNEP = senNEP[SIdent];   // 单个句子的NEP

        graph.addNEtoPhrase(CurNEP,graphs.phrase_map);

        SentenceGraph sentenceGraph(graph);

        sens.add_sentence(sentenceGraph);

        graphs.phrasegraphs.push_back(graph);

    }

    graphs.addCorefertoPhrase(sens.sentence_map,graphs.phrase_map,corefers);


}

void  SaveGraph(std::string path,Graphs graphs){

    std::ofstream os(path,ios::binary);

    cereal::BinaryOutputArchive archive( os );

    archive(graphs);

    os.close();
}

void  LoadGraph(std::string path,Graphs& graphs){

    ifstream ifs(path, std::ios::binary);

    cereal::BinaryInputArchive iarchive(ifs);

    iarchive(graphs);

    ifs.close();
}

ZparTree getPhraseTree(Phrase  phrase,ZparTree sentenceTree){

    ZparTree resTree;

    vector<int> components = phrase.components;

    for(int i=0;i<components.size();i++){

        auto &node=sentenceTree.get_Node(components[i]);

        resTree.add_node(node, true);

    }

    vector<int> slots = phrase.slots;

    for(int j=0;j<slots.size();j++){

        auto &slot_node = sentenceTree.get_Node(slots[j]);

        resTree.add_slot(slot_node, true);
    }

    resTree.set_children_array();

    return resTree;
}

void savePhraseIdx(map<PhraseTransform,int> np_to_id,std::string filepath){
    ofstream out(filepath,ios::out);

    for(auto iter = np_to_id.begin();iter!=np_to_id.end();iter++){
        out<<iter->first.phrasestring<<"\t"<<iter->second<<endl;
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

    const char* input_zpared="/home/wpf/Downloads/input-zpared";
    const char* input_stanford = "/home/wpf/Downloads/input-stanford";

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

       while ((dir = readdir(d)) != NULL)
       {
           if (dir->d_name[0] == '.')
               continue;
           std::string  filename,xmlfile;
           filename = filename+input_zpared+"/"+dir->d_name;

           std::string ss = dir->d_name;
           auto i=ss.find('-');

           xmlfile = xmlfile+input_stanford+"/"+ss.substr(0,i).c_str()+".xml";

           ReadFromZpar(filename,DocId,zpars);

           CommonTool::getInfofromStanford(xmlfile,nerTokens,corefers,DocId);

           DocId++;

       }

       BuildGraph(zpars,graphs,verb_dict,nerTokens,corefers,sentences);   // 可以提出去

       map<PhraseTransform,int> vp_to_id; int startvp=0;

       map<PhraseTransform,int> np_to_id;  int startnp= 0;

       std::map<int,std::map<int,std::map<int,int>>> vp_to_nps;

       //抽出所有的np先
       for(auto iter = graphs.phrasegraphs.begin();iter!=graphs.phrasegraphs.end();iter++){
           auto pg = *iter;

           vector<Phrase>npphrases; //一个句子中的npphrase

           ZparTree sentenceTree = pg.ztree;

           for(auto phiter =pg.pid_to_ident.begin();phiter!=pg.pid_to_ident.end();phiter++){
               auto phident = phiter->second;
               auto phrase = graphs.phrase_map.at(phident);

               if(phrase.isArgument){
                   npphrases.push_back(phrase);
               }
           }
            //sort the phrase
            sort(npphrases.begin(),npphrases.end());

           for(int i=0;i<npphrases.size()-1;i++){
               for(int j=1;j<npphrases.size();j++){
                   Phrase np1 = npphrases[i];
                   Phrase np2 = npphrases[j];

                   int head1 = np1.head;
                   int head2 = np2.head;

                   //找到最小公共祖先
                   int lcaId = sentenceTree.getLca(head1,head2);

                   //获得lca所属的Phrase,使用PhraseGraph中的node_to_phrase
                   int vpId=pg.node_to_phrase.at(lcaId);
                   PhraseIdentity vpIdent = pg.pid_to_ident.at(vpId);
                   Phrase vp = graphs.phrase_map.at(vpIdent);

                   //Phrase transformation
                   ZparTree np1Tree=getPhraseTree(np1,sentenceTree);
                   PhraseTransform np1_trans = PhraseTransform(np1Tree);

                   ZparTree np2Tree=getPhraseTree(np2,sentenceTree);
                   PhraseTransform np2_trans = PhraseTransform(np2Tree);

                   ZparTree vpTree=getPhraseTree(vp,sentenceTree);
                   PhraseTransform vp_trans = PhraseTransform(vpTree);

                   //nppair to id
                   int np1Id;
                   if(np_to_id.find(np1_trans)==np_to_id.end()){
                       np_to_id[np1_trans] = startnp;
                       np1Id = startnp;
                       startnp++;
                   }else{
                           np1Id = np_to_id.at(np1_trans);
                   }

                   int np2Id;
                   if(np_to_id.find(np2_trans)==np_to_id.end()){
                       np_to_id[np2_trans] = startnp;
                       np2Id = startnp;
                       startnp++;
                   }else{
                       np2Id = np_to_id.at(np2_trans);
                   }



                   //vp to id
                   int vphraseId;
                   if(vp_to_id.find(vp_trans)==vp_to_id.end()){
                       vp_to_id[vp_trans]=startvp;
                       vphraseId = startvp;
                       startvp++;
                   }else{
                       vphraseId = vp_to_id.at(vp_trans);
                   }


                   vp_to_nps[vphraseId][np1Id][np2Id]++;



                   //build matrix  size need to compute first
                   //MatrixXd m;
                   //m(npsId,vpId) =


               }
           }
       }
       cout<<startnp<<endl;
       cout<<startvp<<endl;

       savePhraseIdx(np_to_id,"/home/wpf/np_to_id");

       savePhraseIdx(vp_to_id,"/home/wpf/vp_to_id");

       savePatternIdx(vp_to_nps,"/home/wpf/vp_to_nps");

       //遍历所有的np对
       closedir(d);
   }

    return 0;
}