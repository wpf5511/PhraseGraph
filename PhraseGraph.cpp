//
// Created by 王鹏飞 on 16/7/18.
//

#include "PhraseGraph.h"
#include <algorithm>


std::map<std::string,bool>  PhraseGraph::hashPOSOfNoun{
        {"NN", true},
        {"NR", true},
        {"NT", true},
        {"PN", true},
        {"M",  true}
};


std::map<std::string,bool> PhraseGraph::hashPOSOfVerb{
        {"VV", true},
        {"VC", true},
        {"VA", true},
        {"VE", true}
};

std::map<std::string,bool> PhraseGraph::hashPOSOfNPHead{
        {"NN", true},
        {"NR", true},
        {"NT", true},
        {"PN", true},
        {"M",  true}

};


std::map<std::string,bool> PhraseGraph::hashPOSOfNOModifier{
        {"NN", true},
        {"NR", true},
        {"NT", true},
        {"PN", true},
        {"M",  true},
        {"CD", true},
        {"OD", true},
        {"JJ", true},
        {"CC", true}, //和
        {"DEG", true} //的

};
void PhraseGraph::extract_Template() {


    int rootid = ztree.root_id;

    int *global_pid = new int(0);

    Phrase* new_phrase= new Phrase(*global_pid);

    std::map<int,bool> hashIncluded;

    std::vector<int> vvUndernn;// 名词下边的动词


    if(is_noun(rootid)){

        hashIncluded.insert({rootid,true});

        new_phrase->add_component(rootid);

        phrases.insert({new_phrase->id,*new_phrase});
        node_to_phrase.insert({rootid,new_phrase->id});

        dfsnn(rootid,-1,new_phrase,global_pid,hashIncluded,vvUndernn);

    } else{

        phrases.insert({new_phrase->id,*new_phrase});

        dfscm(rootid,-1,new_phrase,global_pid,hashIncluded,vvUndernn);

    }

    int VertexNum = ztree.nodes.size();

    int *visited = new int[VertexNum];

    for(int i=0;i<VertexNum;i++){
        visited[i]=0;
    }

    for(int k=0;k<vvUndernn.size();k++){
        int node_id = vvUndernn[k];
        if(visited[node_id]==0){
            Phrase* vv_phrase= new Phrase(*global_pid+1);
            *global_pid = *global_pid+1;
            dfsvv(node_id,-1,visited,vv_phrase,);
            phrases.insert({new_phrase->id,*vv_phrase});
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

    if(hashPOSOfNoun[pos]||hashPOSOfVerb[pos]){
        return true;
    }else{
        return false;
    }
}

bool PhraseGraph::is_verb(int nodeid){
    auto node = ztree.get_Node(nodeid);

    auto pos = node.pos;

    if(hashPOSOfVerb[pos]){
        return true;
    } else{
        return false;
    }
}

bool PhraseGraph::is_noun(int nodeid){
    auto node = ztree.get_Node(nodeid);

    auto pos = node.pos;

    if(hashPOSOfNoun[pos]){
        return true;
    } else{
        return false;
    }
}

bool PhraseGraph::CanBeModifier(int nodeid){

    auto node = ztree.get_Node(nodeid);

    auto pos = node.pos;

    if(hashPOSOfNOModifier[pos]){
        return true;
    } else{
        return false;
    }
}

bool PhraseGraph::CanBeHead(int nodeid){

    auto node = ztree.get_Node(nodeid);

    auto pos = node.pos;

    if(hashPOSOfNPHead[pos]){
        return true;
    } else{
        return false;
    }
};

void PhraseGraph::dfscm(int start,int previous,Phrase* extract_phrase,int* global_pid,std::map<int,bool>&hasIncluded,std::vector<int>&vvec){

    extract_phrase = &phrases.at(extract_phrase->id);

    auto &cur_node = ztree.get_Node(start);

    int root_id = ztree.root_id;

    std::vector<int> children_id = ztree.get_children(start);

    if(is_end(start)&&start!=root_id){

        extract_phrase->isPredicate = true;

        Phrase* arg_phrase = new Phrase(*global_pid+1);

        *global_pid = *global_pid+1;

        arg_phrase->isArgument = true;

        arg_phrase->add_component(start);

        std::string slot_dep = cur_node.dependency;

        ZparNode slot_node = ZparNode("","",previous,slot_dep, true, true,start);  //注意根是名词的情况

        int slot_id = ztree.nodes.size();

        ztree.add_node(slot_node);

        // arg_phrase->add_slot(slot_id);  //为arg有关的slot添加反向索引

        cur_node.parent_id = slot_id;


        //extract_phrase opt
        extract_phrase->add_slot(slot_id);

        node_to_phrase.insert({slot_id,extract_phrase->id});


        //创建完arg_phrase后,立即insert
        node_to_phrase.insert({start,arg_phrase->id});
        phrases.insert({arg_phrase->id,*arg_phrase});


        if(is_noun(start)){
            //noun is end

            hasIncluded.insert({start,true});

            for(int i=0;i<children_id.size();i++){

                int child_id = children_id[i];

                dfsnn(child_id,start,arg_phrase,global_pid,hasIncluded,vvec);

            }

        }else{
            // verb is end,also is a predicate phrase
            for(int i=0;i<children_id.size();i++){
                int child_id = children_id[i];
                dfscm(child_id,start,arg_phrase,global_pid,hasIncluded,vvec);
            }

        }


    } else{
        extract_phrase->add_component(start);
        node_to_phrase.insert({start,extract_phrase->id});

        for(int i=0;i<children_id.size();i++){
            int child_id = children_id[i];
            dfscm(child_id,start,extract_phrase,global_pid,hasIncluded,vvec);
        }

    }
}

//从arg_nn 开始遍历,找np phrase
void PhraseGraph::dfsnn(int start,int previous,Phrase* arg_phrase,int* global_pid,std::map<int,bool>&hasIncluded,std::vector<int>&vvec){

    arg_phrase = &phrases.at(arg_phrase->id);

    std::vector<int> children_id = ztree.get_children(start);

    //parent is head,current is modifier
    if(CanBeModifier(start)&&hasIncluded[previous]&&!hasIncluded[start]){

        hasIncluded.insert({start,true});

        arg_phrase->add_component(start);

        node_to_phrase.insert({start,arg_phrase->id});
    }
    //current is head,parent is not head
    else if(CanBeHead(start)&&!hasIncluded[previous]&&!hasIncluded[start]){
        //创建新的phrase

        arg_phrase = new Phrase(*global_pid+1);

        *global_pid = *global_pid+1;

        arg_phrase->add_component(start);

        //创建完arg_phrase后,立即insert
        node_to_phrase.insert({start,arg_phrase->id});
        phrases.insert({arg_phrase->id,*arg_phrase});

        hasIncluded.insert({arg_phrase->id,true});

    }
    //不是head也不是modifier,可能是动词或者副词的情况
    else{
        //当前词是动词的话,存储起来,后边调用dfsvv
        if(is_verb(start)){
            vvec.push_back(start);
        }
    }


    for(int i=0;i<children_id.size();i++){

        int child_id = children_id[i];

        dfsnn(child_id,start,arg_phrase,global_pid,hasIncluded,vvec);

    }

}

//从nn下的vv开始遍历,可上可下,到np停止(可能到np的modifier而不是到np的head)
void PhraseGraph::dfsvv(int start,int previous,int *visited,Phrase* extract_phrase,int* pid,std::map<int,bool>hashIncluded){

    auto &cur_node = ztree.get_Node(start);

    std::vector<int> children_id = ztree.get_children(start);

    std::vector<int> adjLists(children_id);

    auto parent_id = cur_node.parent_id;

    adjLists.push_back(parent_id);

    visited[start]=1;

    if(hashIncluded[start]){

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

        ZparNode slot_node = ZparNode("","",slot_parent,slot_dep, true, true,start);

        int slot_id = ztree.nodes.size();

        ztree.add_node(slot_node);

        if(from_direction){
            cur_node.parent_id = slot_id;
        } else{
            auto &previous_node = ztree.get_Node(previous);
            previous_node.parent_id = slot_id;
        }

        extract_phrase->add_slot(slot_id);
        node_to_phrase.insert({slot_id,extract_phrase->id});

        //是arg节点的话  访问后再置为false
        visited[start]=0;
        

    }else{

            extract_phrase->add_component(start);
            node_to_phrase.insert({start,extract_phrase->id});

            for(int i=0;i<adjLists.size();i++){
                if(visited[adjLists.at(i)]==0){
                    dfsvv(adjLists.at(i),start,visited,extract_phrase,pid,hashIncluded);
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

        int node_phrase_id = node_to_phrase[i];

        auto phrase = phrases[node_phrase_id];

        auto flag=phrase.isArgument;

        auto node = ztree.get_Node(i);

        out<<node.lexeme<<"\t"<<node.pos<<"\t"<<node.parent_id<<"\t"
        <<node.dependency<<"\t"<<node.isVirtual<<"\t"<<node.isSlot<<"\t"<<flag<<"\t"<<node_phrase_id<<std::endl;
    }
    return out.str();
}

bool PosComp(int nodeid1,int nodeid2,ZparTree ztree){

    auto node1=ztree.get_Node(nodeid1);
    auto node2=ztree.get_Node(nodeid2);

    return node1.sentense_position<node2.sentense_position;
}
void PhraseGraph::set_content() {
    for(auto itor = phrases.begin();itor!=phrases.end();itor++){
        auto &phrase = itor->second;

        auto comvec = phrase.components;

        std::sort(comvec.begin(),comvec.end(),std::bind(PosComp,std::placeholders::_1,std::placeholders::_2,ztree));

        for(int nodeid:comvec){
            auto word = ztree.get_Node(nodeid).lexeme;
            phrase.content = phrase.content+word+"_";
        }
    }
}