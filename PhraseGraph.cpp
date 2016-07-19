//
// Created by 王鹏飞 on 16/7/18.
//

#include "PhraseGraph.h"
#include <sstream>
#include <fstream>
void PhraseGraph::extract_Template() {
    int VertexNum = ztree.nodes.size();
    int *visited = new int[VertexNum];

    for(int i=0;i<VertexNum;i++){
        visited[i]=0;
    }

    int *global_pid = new int(0);

    for(int i=0;i<VertexNum;i++){

        if(visited[i] ==0&&!is_end(i)) //找一个没被访问过,并且不是arg的开始遍历
        {

                Phrase* new_phrase= new Phrase(*global_pid);

                dfs(i, -1,visited, new_phrase,global_pid);  //起始点的previous置为-1

                phrases.insert({new_phrase->id,*new_phrase});

                *global_pid = *global_pid+1;

                //std::cout<<"hello"<<std::endl;
        }
    }
    for(int j=0;j<phrases.size();j++){
        Phrase *phrase_head = &phrases.at(j);
        find_head(phrase_head);
    }
}

bool PhraseGraph::is_end(int nodeid) {

    auto node = ztree.get_Node(nodeid);

    auto pos = node.pos;

    if(pos=="NN"||pos=="NR"||pos=="NT"){
        return true;
    }else{
        return false;
    }
}

void PhraseGraph::dfs(int start,int previous, int *visited, Phrase *extract_phrase,int *pid) {

    auto &cur_node = ztree.get_Node(start);

    std::vector<int> children_id = ztree.get_children(start);

    std::vector<int> adjLists(children_id);

    auto parent_id = cur_node.parent_id;

    if(parent_id!=-1){
        adjLists.push_back(parent_id);
    }

    visited[start]=1;


    if(is_end(start)){

        extract_phrase->isPredicate = true;
        Phrase *arg_phrase; bool arg_phrase_exist;

        if(node_to_phrase.find(start)==node_to_phrase.end()){
            arg_phrase_exist = false;
            arg_phrase = new Phrase(*pid+1);

            *pid = *pid+1;

            arg_phrase->isArgument = true;

            arg_phrase->add_component(start);


        } else{
            arg_phrase_exist = true;
            int arg_phrase_id = node_to_phrase[start];
            arg_phrase = &phrases.at(arg_phrase_id);
        }

        //判断前一个节点的类型,确定dependency
        std::string slot_dep; int slot_parent;
        bool from_direction; //true means up,false means down
        if(parent_id==previous){
             from_direction = true;
             slot_dep = cur_node.dependency;
             slot_parent = parent_id;
        }else{
            from_direction = false;
            auto previous_node = ztree.get_Node(previous);
            slot_dep = previous_node.dependency;
            slot_parent = start;
        }

        //slot point to arg
        ZparNode slot_node = ZparNode("","",slot_parent,slot_dep, true, true,start);

        int slot_id = ztree.nodes.size();

        ztree.add_node(slot_node);

        arg_phrase->add_slot(slot_id);

        if(from_direction){
            cur_node.parent_id = slot_id;
        } else{
            auto &previous_node = ztree.get_Node(previous);
            previous_node.parent_id = slot_id;
        }

        if(!arg_phrase_exist){

            phrases.insert({arg_phrase->id,*arg_phrase});

            node_to_phrase.insert({start,arg_phrase->id});

        }


        extract_phrase->add_slot(slot_id);
        node_to_phrase.insert({slot_id,extract_phrase->id});

        //是arg节点的话  访问后再置为false
        visited[start]=0;

    } else{
        extract_phrase->add_component(start);
        node_to_phrase.insert({start,extract_phrase->id});

        for(int i=0;i<adjLists.size();i++){
            if(visited[adjLists.at(i)]==0){
                dfs(adjLists.at(i),start,visited,extract_phrase,pid);
            }
        }
    }

}

void PhraseGraph::find_head(Phrase *phrase) {

    auto components_id = phrase->components;
    int head_index=-1;int min_level=21768;
    for(int i=0;i<components_id.size();i++){
        int compid = components_id[i];
        auto node = ztree.get_Node(compid);
        if(node.level<min_level){
            min_level = node.level;
            head_index = node.id;
        }
    }
    phrase->head = head_index;
}

PhraseGraph::PhraseGraph(ZparTree ztree) {
    this->ztree = ztree;
}


std::string PhraseGraph::to_string() {

    std::ostringstream out;
    for(int i=0;i<ztree.nodes.size();i++){
        auto node = ztree.get_Node(i);
        out<<node.id<<"\t"<<node.lexeme<<"\t"<<node.pos<<"\t"<<node.parent_id<<"\t"
        <<node.dependency<<"\t"<<node.isVirtual<<"\t"<<node.isSlot<<"\t"<<node.link<<std::endl;
    }
    return out.str();
}

/*void Graphs::load_Graphs(std::string graphfile) {

    std::ifstream infile(graphfile);

    ZparTree ztree;

    PhraseGraph pg;
    std::map<int,Phrase> phrases;
    std::map<int,int> node_to_map;

    std::string lexeme;std::string pos;std::string parent_id;std::string dependency;
    bool isVirtual;bool isSlot;int link;int level;

    std::string line;

    bool phrase_step;int inter_pos;
    while(getline(infile,line)){
        if(line.empty()){
            PhraseGraph single_phrase;
            single_phrase.ztree = ztree;
            phrasegraphs.push_back(single_phrase);
            ztree =ZparTree();
        } else{
            std::istringstream is(line);
            is>>lexeme>>pos>>parent_id>>dependency>>isVirtual>>isSlot>>link;
            ztree.add_node(ZparNode(lexeme,pos,stoi(parent_id),dependency,isVirtual,isSlot,link));

            if(line=="phrase"){
                phrase_step = true;
                inter_pos=0;
            }
            std::vector<int> compos; std::vector<int> slots;
            if(phrase_step){
                int id;int head;bool ispre;bool isarg;int nargs;
                switch(inter_pos){
                    case 0:
                        is>>id;
                        break;
                    case 1:
                        is>>id>>head>>ispre>>isarg>>nargs;
                        break;
                    case 2:

                        break;
                    case 3:
                        break;
                }
            }
        }
    }
}


void Graphs::save_Graphs(std::string graphfile) {

    std::ofstream  outfile(graphfile);

    for(int i=0;i<phrasegraphs.size();i++){
        auto Single_graph = phrasegraphs[i];
        auto Graph_string = Single_graph.to_string();
        outfile<<Graph_string;

        outfile<<"phrases"<<std::endl;

        //序列化phrase
        for(int j=0;j<Single_graph.phrases.size();j++){
            outfile<<j<<std::endl;
            auto res_map = Single_graph.phrases.at(j);

            //output phrase member
            outfile<<res_map.id<<"\t"<<res_map.head<<"\t"<<res_map.isPredicate<<"\t"
            <<res_map.isArgument<<"\t"<<res_map.nargs<<std::endl;

            for(auto item:res_map.components){
                outfile<<item<<"\t";
            }  outfile<<std::endl;

            for(auto slot:res_map.slots){
                outfile<<slot<<"\t";
            }outfile<<std::endl;
        }

        //序列化node_to_phrase

        outfile<<"node_to_phrase"<<std::endl;

        for(auto map_item:Single_graph.node_to_phrase){
            outfile<<map_item.first<<"\t"<<map_item.second<<endl;
        }


        //两个phrase之间出现空行
        outfile<<std::endl;
    }
}
*/