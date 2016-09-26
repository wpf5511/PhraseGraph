//
// Created by 王鹏飞 on 16/6/23.
//

#ifndef INFER_FROM_TREE_ZPARTREE_H
#define INFER_FROM_TREE_ZPARTREE_H

#include <map>
#include <vector>
#include <set>
#include <string>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
class ZparNode{
    public:

        ZparNode()= default;


        ZparNode(std::string lexeme,std::string pos,int parent_id,std::string dependency,int idInDocument,int idInSentence,
                 bool isVirtual=false,bool isSlot= false,int link=-2,int level=-2);

        ZparNode(const ZparNode &node);

        template <class Archive>
        void serialize( Archive & ar )
         {
            ar(id,level,sentense_position,lexeme,pos,parent_id,dependency,isVirtual,isSlot,link);
         }


        int idInDocument;
        int idInSentence;
        int idInPhrase;

        int id;  //word position in the sentence
        int level;
        int sentense_position;
        std::string lexeme;  //word
        std::string pos;   //pos tag
        int parent_id;
        std::string dependency;  //dependency relation

        bool isVirtual;

        bool isSlot;
        int link;

        //重现句子时候需要
        bool operator <(const ZparNode&other) const{
            return this->sentense_position<other.sentense_position;
        }


};


class ZparTree {
public:
    int idInDocument;

    int idInSentence;

    std::vector<ZparNode> nodes;
    std::map<int,std::vector<int>> children_array;
    int root_id;
    int min_level = 100;

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(nodes,children_array,root_id);
    }



    ZparTree()= default;
    ZparTree(const ZparTree& cztree);
    void add_node(ZparNode node);
    void add_node(ZparNode node,int sentence_position);
    void add_node(ZparNode node3,bool has_id);
    void add_slot(ZparNode node3,bool has_id);

    void set_children_array();
    void set_level(int id,int level);
    ZparNode& get_Node(int id);
    std::vector<int> get_children(int id);
    std::string to_sentence();

    void preprocessing(std::set<std::string>verb_dict);

    std::vector<int> getPathFromRoot(int id);

    int  getLca(int id1,int id2);
};


#endif //INFER_FROM_TREE_ZPARTREE_H
