#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <thread>
#include "ZparTree.h"
#include "PhraseGraph.h"
#include "PhraseTransform.h"
#include "init.h"
#include <cereal/archives/xml.hpp>
#include <dirent.h>
#include <time.h>
#include <boost/algorithm/string.hpp>

using namespace boost::algorithm;

using namespace std;



const char* DIRUP = ">";

const char* DIRDOWN = "<";

const int THRED_NUM = 15;

const int MAX_PATH_LEN = 4;
struct Path{
    int set_x;
    int X;
    vector<int>hx;
    int lcaId;
    int Y;
    int set_y;
    vector<int>hy;
    Path(int head1,vector<int>path1,int lca,int head2,vector<int>path2){
        this->X = head1;
        this->hx = path1;
        this->lcaId = lca;
        this->Y = head2;
        this->hy = path2;
        this->set_x=-1;
        this->set_y=-1;
    }

    Path(const Path&other){
        this->X = other.X;
        this->hx = other.hx;
        this->lcaId = other.lcaId;
        this->Y = other.Y;
        this->hy = other.hy;
        this->set_x = other.set_x;
        this->set_y = other.set_y;
    }

    Path(const Path&other,int set_x,int set_y){
        this->X = other.X;
        this->hx = other.hx;
        this->lcaId = other.lcaId;
        this->Y = other.Y;
        this->hy = other.hy;

        this->set_x = set_x;
        this->set_y = set_y;
    }
};

std::string edge_to_string(int id,ZparTree ztree,bool isHead=false){
    auto node = ztree.get_Node(id);

    int w_id = node.lexeme;
    int p_id = node.pos;
    int d_id = node.dependency;

    std::string dep = isHead?"ROOT":dep2id.right.at(d_id);
    ostringstream os;

    os<<word2id.right.at(w_id)<<"/"<<pos2id.right.at(p_id)<<"/"
    <<dep;
    return  os.str();
}

std::string argument_to_string(int id,std::string edge_name,ZparTree ztree,bool isHead){

    auto node = ztree.get_Node(id);

    int w_id = node.lexeme;
    int p_id = node.pos;
    int d_id = node.dependency;

    std::string dep = isHead?"ROOT":dep2id.right.at(d_id);
    ostringstream os;

    os<<edge_name<<"/"<<pos2id.right.at(p_id)<<"/"<<dep2id.right.at(d_id);

    return os.str();

    }

bool compare_position(int head,int modifier,ZparTree ztree){

    auto head_node = ztree.get_Node(head);
    auto modify_node = ztree.get_Node(modifier);

    return head_node.sentense_position>modify_node.sentense_position;
}

//direction is correct return true
bool  check_direction(int lca,std::vector<int>hs,bool isLeft,ZparTree ztree){
    hs.insert(hs.begin(),lca);

    for(int i=0;i<hs.size()-1;i++){
        if(compare_position(hs[i],hs[i+1],ztree)!=isLeft){
            return false;
        }
    }
    return true;
}


std::vector<Path> get_satellite_links(Path p,ZparTree ztree){

    std::vector<Path> sateres;
    sateres.push_back(p);


    auto xnode = ztree.get_Node(p.X);

    vector<int> Xchild=ztree.get_children(p.X);

    if(Xchild.size()>0){
        sort(Xchild.begin(),Xchild.end(),std::bind(PosComp,std::placeholders::_1,std::placeholders::_2,ztree));

        auto child_node = ztree.get_Node(Xchild[0]);

        if(child_node.sentense_position<xnode.sentense_position){
            sateres.push_back(Path(p,Xchild[0],-1));
        }
    }

    auto ynode = ztree.get_Node(p.Y);

    vector<int> Ychild=ztree.get_children(p.Y);

    if(Ychild.size()>0){
        //children  sort by  sentence pos asc
        sort(Ychild.begin(),Ychild.end(),std::bind(PosComp,std::placeholders::_1,std::placeholders::_2,ztree));

        for(int i=0;i<Ychild.size();i++){
            auto child_node = ztree.get_Node(Ychild[i]);
            if(child_node.sentense_position>xnode.sentense_position){
                sateres.push_back(Path(p,-1,Ychild[i]));
            }
        }
    }

    return sateres;

}


std::string clean_path(Path p1,ZparTree ztree){
    const char* dir_x;
    const char* dir_y;
    std::string lch_str;
    bool XisHead= false;
    bool YisHead= false;
    std::string set_path_x;
    std::string set_path_y;

    int len_path=0;

    if(p1.set_x!=-1){
        set_path_x = edge_to_string(p1.set_x,ztree)+DIRDOWN;
        len_path++;
    }

    if(p1.set_y!=-1){
        set_path_y = DIRUP+edge_to_string(p1.set_y,ztree);
        len_path++;
    }

    if(p1.lcaId==p1.X){
         dir_x= "";
         dir_y = DIRDOWN;
        XisHead = true;
    } else if(p1.lcaId==p1.Y){
        dir_x = DIRUP;
        dir_y = "";
        YisHead = true;
    } else{
        lch_str = edge_to_string(p1.lcaId,ztree,true);
        dir_x = DIRUP;
        dir_y = DIRDOWN;
    }

    std::string cleaned_path;
    std::string hx_path;
    std::string hy_path;
    for(auto iter = p1.hx.begin();iter!=p1.hx.end();iter++){

        hx_path+=edge_to_string(*iter,ztree)+DIRUP;
        len_path++;
    }

    for(auto it = p1.hy.begin();it!=p1.hy.end();it++){
        hy_path+=DIRDOWN+edge_to_string(*it,ztree);
        len_path++;
    }

    len_path = len_path+1;  // add lca lenth

    if(len_path<=MAX_PATH_LEN) {
        cleaned_path = set_path_x+argument_to_string(p1.X,"X",ztree,XisHead)+dir_x+hx_path+lch_str+hy_path+dir_y+argument_to_string(p1.Y,"Y",ztree,YisHead)+set_path_y;
    }

    return  cleaned_path;

}

bool is_Entity(int posid){
    //|pos2id.right.at(posid)=="NN"||pos2id.right.at(posid)=="NR"||pos2id.right.at(posid)=="PN"
    if(enpos2id.right.at(posid).substr(0,2)=="NN"){
        return true;
    } else {
        return false;
    }
}


std::vector<string> DIRT_From_Zpar(ZparTree ztree){

    auto Tree = ztree;

    auto nodes = Tree.nodes;

    sort(nodes.begin(),nodes.end());

    int ns = nodes.size();

    std::vector<string> res;

    for(int j=0;j<ns-1;j++){
        if(!is_Entity(nodes[j].pos))
            continue;
        for(int k=j+1;k<ns;k++){
            if(!is_Entity(nodes[k].pos))
                continue;
            int  head1 = nodes[j].id;
            int  head2 = nodes[k].id;

            vector<int>path1 = Tree.getPathFromRoot(head1);
            vector<int>path2 = Tree.getPathFromRoot(head2);

            auto lca_info  = Tree.getLca(path1,path2);
            vector<int>hx;
            vector<int>hy;
            if(std::get<0>(lca_info)+1<=path1.size()){
                hx.assign(path1.begin()+std::get<0>(lca_info)+1,path1.end());
                if(!check_direction(std::get<1>(lca_info),hx,true,ztree))
                    continue;
                std::reverse(hx.begin(),hx.end());
            }

            if(std::get<0>(lca_info)+1<=path2.size()){
                hy.assign(path2.begin()+std::get<0>(lca_info)+1,path2.end());
                if(!check_direction(std::get<1>(lca_info),hy,true,ztree))
                    continue;
            }


            Path p(head1,hx,std::get<1>(lca_info),head2,hy);

            std::vector<Path> sate_str=get_satellite_links(p,ztree);

            for(auto p:sate_str){

                std::string path_string = clean_path(p,ztree);

                if(!path_string.empty()){

                    int X_lexeme = ztree.get_Node(head1).lexeme;

                    int Y_lexeme = ztree.get_Node(head2).lexeme;

                    std::string triple_string = word2id.right.at(X_lexeme)+"\t"+word2id.right.at(Y_lexeme)+"\t"+path_string;

                    res.push_back(triple_string);

                }
            }
        }
    }

    return res;

}

void ReadDict(std::string path,set<std::string>&verb_dict){

    ifstream verb_file(path);

    string word;

    while(getline(verb_file,word)){
        verb_dict.insert(word);
    }

    verb_file.close();

}

void write_triple(std::vector<std::string>triple,ofstream& out){

    for(int i=0;i<triple.size();i++){
        out<<triple[i]<<std::endl;
    }

}

void FindTreeStart(ifstream& in){

    std::string line;
    while(getline(in,line)){
        if(line=="\n")
            break;
    }
}
void ReadFromZpar(std::string path,int DocId,ofstream& out){

    word2id.insert({"Unknown",0});
    word2id.insert({"*",1});

    pos2id.insert({"*",0});
    dep2id.insert({"*",0});

    ifstream Zparfile(path,ios::in);

    /*Zparfile.seekg(std::streamoff(0),std::ios::end);
    int64_t  filesize  = Zparfile.tellg();   // byte

    Zparfile.clear();
    Zparfile.seekg(std::streampos(tid*filesize/THRED_NUM));*/

    string line;
    string lexeme,pos,parent_id,dependency;

    ZparTree *zparTree=new ZparTree();

    int SentenceId = 0;

    while(getline(Zparfile,line)){
        if(line.empty()){
            zparTree->set_children_array();
            zparTree->idInDocument = DocId;
            zparTree->idInSentence = SentenceId;
            //zpars.push_back(*zparTree);  // store or not store;
            std::vector<string> sen_res = DIRT_From_Zpar(*zparTree);

            write_triple(sen_res,out);

            if(SentenceId%10000==0){
                cout<<"SentenceId:"<<SentenceId<<endl;
            }

            delete zparTree;
            zparTree = new ZparTree();
            SentenceId++;
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

            zparTree->add_node(ZparNode(word2id.left.at(lexeme),pos2id.left.at(pos),stoi(parent_id),dep2id.left.at(dependency),DocId,SentenceId));
        }
    }

    Zparfile.close();
}

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
                                      s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}


void ReadFromStanford(std::string path,int DocId,ofstream& out){
    word2id.insert({"Unknown",0});
    word2id.insert({"*",1});

    enpos2id.insert({"*",0});
    dep2id.insert({"*",0});

    ifstream Stanfordfile(path,ios::in);

    string line;
    string dependency,headinfo,headid,modinfo,modid;

    string lexeme,pos;

    ZparTree *zparTree=new ZparTree();

    int SentenceId = 0;

    std::vector<std::string> v;

    while(getline(Stanfordfile,line)){
        if(line.empty()){
            zparTree->set_children_array();
            zparTree->idInDocument = DocId;
            zparTree->idInSentence = SentenceId;
            //zpars.push_back(*zparTree);  // store or not store;
            std::vector<string> sen_res = DIRT_From_Zpar(*zparTree);

            write_triple(sen_res,out);

            if(SentenceId%10000==0){
                cout<<"SentenceId:"<<SentenceId<<endl;
            }

            delete zparTree;
            zparTree = new ZparTree();
            SentenceId++;
        }else if(is_number(line)){
            continue;
        }
        else{
            istringstream is(line);

            is>>dependency>>headinfo>>headid>>modinfo>>modid;

            split(v,modinfo, boost::is_any_of("/"));

            lexeme = v[0];pos = v[1];
            if(word2id.left.find(lexeme)==word2id.left.end()){
                word2id.insert({lexeme,start_wid++});
            }

            if(dep2id.left.find(dependency)==dep2id.left.end()){
                dep2id.insert({dependency,start_did++});
            }

            if(enpos2id.left.find(pos)==enpos2id.left.end()){
                enpos2id.insert({pos,start_pid++});
            }

            zparTree->add_node(ZparNode(word2id.left.at(lexeme),enpos2id.left.at(pos),stoi(headid)-1,dep2id.left.at(dependency),DocId,SentenceId));
        }
    }

    Stanfordfile.close();

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

       std::ofstream out("/home/wpf/triple",ios::out);

       while ((dir = readdir(d)) != NULL)
       {
           if (dir->d_name[0] == '.')
               continue;
           std::string  filename,xmlfile;
           filename = filename+input_zpared+"/"+dir->d_name;

           std::string ss = dir->d_name;

           xmlfile = xmlfile+input_stanford+"/"+ss+".xml";

           //ReadFromZpar(filename,DocId,out);
           ReadFromStanford(filename,DocId,out);

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
