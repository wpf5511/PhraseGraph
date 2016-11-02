//
// Created by wpf on 10/23/16.
//

#include "init.h"

using namespace std;


int start_wid=2;

int start_did=1;

const char* DIRUP = ">";

const char* DIRDOWN = "<";

bimap  word2id;

bimap  dep2id;

bimap& pos2id(){

    static bimap pos2id = boost::assign::list_of< bimap::relation >
            ("AD",1)
            ("AS",2)
            ("BA",3)
            ("CC",4)
            ("CD",5)
            ("CS",6)
            ("DEC",7)
            ("DEG",8)
            ("DER",9)
            ("DEV",10)
            ("DT",11)
            ("ETC",12)
            ("FW",13)
            ("IJ",14)
            ("JJ",15)
            ("LB",16)
            ("LC",17)
            ("M",18)
            ("MSP",19)
            ("NN",20)
            ("NR",21)
            ("NT",22)
            ("OD",23)
            ("ON",24)
            ("P",25)
            ("PN",26)
            ("PU",27)
            ("SB",28)
            ("SP",29)
            ("VA",30)
            ("VC",31)
            ("VE",32)
            ("VV",33);

    return pos2id;
}





void ReadFromZpar(std::string path,int DocId,ofstream& out){

    word2id.insert({"Unknown",0});
    word2id.insert({"*",1});

    pos2id().insert({"*",0});
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

	    write_quadruple(sen_res,out,SentenceId);

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

            zparTree->add_node(ZparNode(word2id.left.at(lexeme),pos2id().left.at(pos),stoi(parent_id),dep2id.left.at(dependency),DocId,SentenceId));
        }
    }

    Zparfile.close();
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
                if(!check_direction(std::get<1>(lca_info),hy, false,ztree))
                    continue;
            }

            if(!hx.empty()){
                hx.erase(hx.begin());
            }

            if(!hy.empty()){
                hy.pop_back();
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

std::string DIRT_X_Y(ZparTree ztree){

    auto Tree = ztree;

    auto nodes = Tree.nodes;

    sort(nodes.begin(),nodes.end());

    int ns = nodes.size();

    std::string  res;

    for(int j=0;j<ns-1;j++){
        if(!is_X(nodes[j].lexeme))
            continue;
        for(int k=j+1;k<ns;k++){
            if(!is_Y(nodes[k].lexeme))
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
                if(!check_direction(std::get<1>(lca_info),hy,false,ztree))
                    continue;
            }

            //hx contains x and hy contains y
            if(!hx.empty()){
                hx.erase(hx.begin());
            }

            if(!hy.empty()){
                hy.pop_back();
            }


            Path p(head1,hx,std::get<1>(lca_info),head2,hy);

            std::string path_string = clean_path(p,ztree);

            res = path_string;
        }
    }

    return res;
}

void write_triple(std::vector<std::string>triple,ofstream& out){

    for(int i=0;i<triple.size();i++){
        out<<triple[i]<<std::endl;
    }

}

void write_quadruple(std::vector<std::string>triple,ofstream& out,int sentId){

    for(int i=0;i<triple.size();i++){
	out<<triple[i]<<"\t"<<sentId<<std::endl;
    }
}

bool is_Entity(int posid){
    if(pos2id().right.at(posid)=="NN"||pos2id().right.at(posid)=="NR"){
        return true;
    } else {
        return false;
    }
}

bool is_X(int wordid){
    if(word2id.right.at(wordid)=="X"||word2id.right.at(wordid)=="x"){
        return true;
    }else {
        return false;
    }
}

bool is_Y(int wordid){

    if(word2id.right.at(wordid)=="Y"||word2id.right.at(wordid)=="y"){
        return true;
    }else {
        return false;
    }
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

bool compare_position(int head,int modifier,ZparTree ztree){

    auto head_node = ztree.get_Node(head);
    auto modify_node = ztree.get_Node(modifier);

    return head_node.sentense_position>modify_node.sentense_position;
}

std::vector<Path> get_satellite_links(Path p,ZparTree ztree){

    std::vector<Path> sateres;

    sateres.push_back(p);


    auto xnode = ztree.get_Node(p.X);

    vector<int> Xchild=ztree.get_children(p.X);

    if(Xchild.size()>0){
        //children  sort by  sentence pos asc
        sort(Xchild.begin(),Xchild.end(),std::bind(PosComp,std::placeholders::_1,std::placeholders::_2,ztree));

        for(auto it = Xchild.rbegin();it!=Xchild.rend();it++){
            auto child_node = ztree.get_Node(*it);
            if(child_node.sentense_position<xnode.sentense_position){
                sateres.push_back(Path(p,*it,-1));
                break;
            }
        }
    }

    auto ynode = ztree.get_Node(p.Y);

    vector<int> Ychild=ztree.get_children(p.Y);

    if(Ychild.size()>0){
        //children  sort by  sentence pos asc
        sort(Ychild.begin(),Ychild.end(),std::bind(PosComp,std::placeholders::_1,std::placeholders::_2,ztree));

        for(int i=0;i<Ychild.size();i++){
            auto child_node = ztree.get_Node(Ychild[i]);
            if(child_node.sentense_position>ynode.sentense_position){
                sateres.push_back(Path(p,-1,Ychild[i]));
                break;
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

std::string edge_to_string(int id,ZparTree ztree,bool isHead){
    auto node = ztree.get_Node(id);

    int w_id = node.lexeme;
    int p_id = node.pos;
    int d_id = node.dependency;

    std::string dep = isHead?"ROOT":dep2id.right.at(d_id);
    ostringstream os;

    os<<word2id.right.at(w_id)<<"/"<<pos2id().right.at(p_id)<<"/"
    <<dep;
    return  os.str();
}

std::string argument_to_string(int id,std::string edge_name,ZparTree ztree,bool isHead){

    auto node = ztree.get_Node(id);

    int w_id = node.lexeme;
    int p_id = node.pos;
    int d_id = node.dependency;

    std::string dep = isHead?"ROOT":dep2id.right.at(0);
    ostringstream os;

    // entity pos:NN   dep:*
    os<<edge_name<<"/"<<pos2id().right.at(20)<<"/"<<dep;

    return os.str();

}





