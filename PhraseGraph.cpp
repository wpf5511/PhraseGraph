//
// Created by 王鹏飞 on 16/7/18.
//

#include "PhraseGraph.h"
#include <cmath>
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
void PhraseGraph::extract_Phrases(std::map<PhraseIdentity,Phrase*> &phrase_map) {


    int rootid = ztree.root_id;

    int *global_pid = new int(0);

    Phrase* new_phrase= new Phrase(*global_pid);

    std::map<int,bool> hashIncluded;

    std::vector<int> vvUndernn;// 名词下边的动词


    if(is_noun(rootid)){

        hashIncluded.insert({rootid,true});

        new_phrase->add_component(rootid);

        //phrases.insert({new_phrase->id,*new_phrase});   former structure

        auto Pident = getPhraseIdent(new_phrase->id);  // latter structure

        phrase_map.insert({Pident,new_phrase});        // latter structure

        pid_to_ident.insert({new_phrase->id,Pident});   //latter structure

        node_to_phrase.insert({rootid,new_phrase->id});

        dfsnn(rootid,-1,new_phrase,global_pid,hashIncluded,vvUndernn,phrase_map);

    } else{

        // phrases.insert({new_phrase->id,*new_phrase});  former structure

        auto Pident = getPhraseIdent(new_phrase->id);  // latter structure

        phrase_map.insert({Pident,new_phrase});        // latter structure

        pid_to_ident.insert({new_phrase->id,Pident});   //latter structure


        dfscm(rootid,-1,new_phrase,global_pid,hashIncluded,vvUndernn,phrase_map);

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
            dfsvv(node_id,-1,visited,vv_phrase,global_pid,hashIncluded,phrase_map);
            // phrases.insert({vv_phrase->id,*vv_phrase});  former structure

            auto Pident = getPhraseIdent(vv_phrase->id);  // latter structure

            phrase_map.insert({Pident,vv_phrase});        // latter structure

            pid_to_ident.insert({vv_phrase->id,Pident});   //latter structure


        }
    }



    // former structure
    /*for(int j=0;j<phrases.size();j++){
        Phrase *phrase_head = &phrases.at(j);
        find_head(phrase_head);
    }*/


    //phrase_id from 0 to n
    for(int j=0;j<pid_to_ident.size();j++){

        auto Pident =pid_to_ident.at(j);

        Phrase *phrase_head = phrase_map.at(Pident);

        find_head(phrase_head);
    }


    set_content(phrase_map);
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

void PhraseGraph::dfscm(int start,int previous,Phrase* extract_phrase,int* global_pid,std::map<int,bool>&hasIncluded,std::vector<int>&vvec,std::map<PhraseIdentity,Phrase*> &phrase_map){

    auto extract_phrase_ident = getPhraseIdent(extract_phrase->id);

    // extract_phrase = &phrases.at(extract_phrase->id);  former structure

    extract_phrase = phrase_map.at(extract_phrase_ident);  // latter structure


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

        int DocId = ztree.idInDocument; int SenId = ztree.idInSentence;

        ZparNode slot_node = ZparNode("*","*",previous,slot_dep,DocId,SenId,true,true,start);  //注意根是名词的情况

        int slot_id = ztree.nodes.size();

        ztree.add_node(slot_node,start);

        // arg_phrase->add_slot(slot_id);  //为arg有关的slot添加反向索引

        cur_node.parent_id = slot_id;


        //extract_phrase opt
        extract_phrase->add_slot(slot_id);

        node_to_phrase.insert({slot_id,extract_phrase->id});


        //创建完arg_phrase后,立即insert
        node_to_phrase.insert({start,arg_phrase->id});


        //phrases.insert({arg_phrase->id,*arg_phrase});


        auto Pident = getPhraseIdent(arg_phrase->id);  // latter structure

        phrase_map.insert({Pident,arg_phrase});        // latter structure

        pid_to_ident.insert({arg_phrase->id,Pident});   //latter structure



        if(is_noun(start)){
            //noun is end

            hasIncluded.insert({start,true});

            for(int i=0;i<children_id.size();i++){

                int child_id = children_id[i];

                dfsnn(child_id,start,arg_phrase,global_pid,hasIncluded,vvec,phrase_map);

            }

        }else{
            // verb is end,also is a predicate phrase
            for(int i=0;i<children_id.size();i++){
                int child_id = children_id[i];
                dfscm(child_id,start,arg_phrase,global_pid,hasIncluded,vvec,phrase_map);
            }

        }


    } else{
        extract_phrase->add_component(start);
        node_to_phrase.insert({start,extract_phrase->id});

        for(int i=0;i<children_id.size();i++){
            int child_id = children_id[i];
            dfscm(child_id,start,extract_phrase,global_pid,hasIncluded,vvec,phrase_map);
        }

    }
}

//从arg_nn 开始遍历,找np phrase
void PhraseGraph::dfsnn(int start,int previous,Phrase* arg_phrase,int* global_pid,std::map<int,bool>&hasIncluded,std::vector<int>&vvec,std::map<PhraseIdentity,Phrase*> &phrase_map){

    auto arg_phrase_ident = getPhraseIdent(arg_phrase->id);

    //arg_phrase = &phrases.at(arg_phrase->id);

    arg_phrase = phrase_map.at(arg_phrase_ident);  // latter structure

    std::vector<int> children_id = ztree.get_children(start);

    //parent is head,current is modifier
    if(CanBeModifier(start)&&hasIncluded.find(previous)!=hasIncluded.end()&&hasIncluded.find(start)==hasIncluded.end()){

        hasIncluded.insert({start,true});

        arg_phrase->add_component(start);

        node_to_phrase.insert({start,arg_phrase->id});
    }
    //current is head,parent is not head
    else if(CanBeHead(start)&&hasIncluded.find(previous)==hasIncluded.end()&&hasIncluded.find(start)==hasIncluded.end()){
        //创建新的phrase

        arg_phrase = new Phrase(*global_pid+1);

        *global_pid = *global_pid+1;

        arg_phrase->add_component(start);

        //创建完arg_phrase后,立即insert
        node_to_phrase.insert({start,arg_phrase->id});

        // phrases.insert({arg_phrase->id,*arg_phrase});  former structure

        auto Pident = getPhraseIdent(arg_phrase->id);  // latter structure

        phrase_map.insert({Pident,arg_phrase});        // latter structure

        pid_to_ident.insert({arg_phrase->id,Pident});   //latter structure


        hasIncluded.insert({start,true});

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

        dfsnn(child_id,start,arg_phrase,global_pid,hasIncluded,vvec,phrase_map);

    }

}

//从nn下的vv开始遍历,可上可下,到np停止(可能到np的modifier而不是到np的head)
void PhraseGraph::dfsvv(int start,int previous,int *visited,Phrase* extract_phrase,int* pid,std::map<int,bool>hashIncluded, std::map<PhraseIdentity,Phrase*> &phrase_map){

    auto &cur_node = ztree.get_Node(start);

    std::vector<int> children_id = ztree.get_children(start);

    std::vector<int> adjLists(children_id);

    auto parent_id = cur_node.parent_id;

    adjLists.push_back(parent_id);

    visited[start]=1;

    if(hashIncluded.find(start)!=hashIncluded.end()){

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

        int DocId = ztree.idInDocument; int SenId = ztree.idInSentence;

        ZparNode slot_node = ZparNode("*","*",slot_parent,slot_dep, DocId,SenId,true, true,start);

        int slot_id = ztree.nodes.size();

        ztree.add_node(slot_node,start);

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
                    dfsvv(adjLists.at(i),start,visited,extract_phrase,pid,hashIncluded,phrase_map);
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

PhraseGraph::PhraseGraph(ZparTree& zparTree):ztree(zparTree) {
    this->ztree = ztree;
    this->idInDocument = ztree.idInDocument;
    this->idInSentence = ztree.idInSentence;
    this->sen_content = ztree.to_sentence();

}


std::string PhraseGraph::to_string(const std::map<PhraseIdentity,Phrase> &phrase_map) {

    std::ostringstream out;
    for(int i=0;i<ztree.nodes.size();i++){

        int node_phrase_id = node_to_phrase[i];

        auto node_Pident = getPhraseIdent(node_phrase_id);

        Phrase phrase = phrase_map.at(node_Pident);

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

void PhraseGraph::set_content(std::map<PhraseIdentity,Phrase*> &phrase_map) {


    for(auto itor = pid_to_ident.begin();itor!=pid_to_ident.end();itor++){

        auto Pident =itor->second;

        Phrase* phrase = phrase_map.at(Pident);

        auto comvec = phrase->components;

        std::sort(comvec.begin(),comvec.end(),std::bind(PosComp,std::placeholders::_1,std::placeholders::_2,ztree));

        for(int nodeid:comvec){
            auto word = ztree.get_Node(nodeid).lexeme;
            phrase->content = phrase->content+word+"_";
        }
    }
}


void PhraseGraph::addNEtoPhrase(NEPMAP neps,std::map<PhraseIdentity,Phrase> &phrase_map) {

    for(auto it = pid_to_ident.begin();it!=pid_to_ident.end();it++){

        auto Pident = it->second;

        auto &phrase = phrase_map.at(Pident);


        if(phrase.isArgument== false){
            continue;
        } else{
            int  phead = phrase.head;
            std::string pheadstring = ztree.get_Node(phead).lexeme;

            int maxlenlcs = 0;
            NEPMAP::iterator maxlenitor;
            for(auto it2= neps.begin();it2!=neps.end();it2++){
                int neHead = it2->first;
                std::string neHeadstring = it2->second.headstring;

                std::wstring wphead = CommonTool::s2ws(pheadstring);

                std::wstring wneHead = CommonTool::s2ws(neHeadstring);

                if(std::fabs(phead-neHead)<=3&&wphead.back()==wneHead.back()){
                    phrase.entityType = it2->second.entityType;
                    phrase.isNE = true;
                    neps.erase(it2);
                    break;
                }else{
                    std::string pContent = phrase.content;
                    std::string neContent = it2->second.phrase;

                    int commonlen = CommonTool::lcs(CommonTool::s2ws(pContent),CommonTool::s2ws(neContent));

                    if(commonlen>maxlenlcs){
                        maxlenlcs = commonlen;
                        maxlenitor = it2;
                    }
                }
            }
            if(maxlenlcs>0){
                phrase.entityType = maxlenitor->second.entityType;
                phrase.isNE = true;
                neps.erase(maxlenitor);
            }
        }
    }
}

void Graphs::addCorefertoPhrase(std::map<SentenceIdentity, SentenceGraph> sentence_map,
                                     std::map<PhraseIdentity, Phrase> &phrase_map, std::vector<Corefer> corefers) {
    for(int i=0;i<corefers.size();i++){
        auto corefer = corefers[i];
        SentenceIdentity  sIden(corefer.Documentid,corefer.sentenceid);
        SentenceGraph  sen = sentence_map.at(sIden);
        int phrases_num = sen.phrases_num;

        int maxlenlcs=0; PhraseIdentity corefer_pIden;

        for(auto  pid=0;pid<phrases_num;pid++){

            PhraseIdentity pIden(corefer.Documentid,corefer.sentenceid,pid);

            auto sen_phrase = phrase_map.at(pIden);

            if(sen_phrase.isArgument== false){
                continue;
            }
            else{
                int  phead = sen_phrase.head;

                int coreHead = corefer.headIndex;

                if(std::fabs(phead-coreHead)<=3){
                    std::string pContent = sen_phrase.content;
                    std::string crContent = corefer.text;

                    int commonlen = CommonTool::lcs(CommonTool::s2ws(pContent),CommonTool::s2ws(crContent));

                    if(commonlen>maxlenlcs){
                        maxlenlcs = commonlen;
                        corefer_pIden = pIden;
                    }
                }
            }
        }
        PhraseIdentity  referedIden;

        if(maxlenlcs>0){
            if(corefer.isRepresentative){
                referedIden = corefer_pIden;
                auto &corefer_phrase = phrase_map.at(referedIden);
                corefer_phrase.isRepresent = true;
            }else{
                auto &corefer_phrase = phrase_map.at(corefer_pIden);
                corefer_phrase.coreferPhrase = referedIden;
            }
        }
    }

}

