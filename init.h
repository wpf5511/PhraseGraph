//
// Created by wpf on 9/24/16.
//

#ifndef PHRASEGRAPH_INIT_H
#define PHRASEGRAPH_INIT_H

#endif //PHRASEGRAPH_INIT_H

#include <string>
#include <iostream>
#include <boost/bimap.hpp>
#include <boost/assign.hpp>
#include <fstream>
#include <iostream>
#include "ZparTree.h"
#include "PhraseGraph.h"


using  namespace std;

typedef boost::bimap<std::string, int> bimap;

extern  int start_wid;
extern  int start_did;


 extern bimap  word2id;

 extern bimap  dep2id;


 bimap& pos2id();

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

extern const char* DIRUP;

extern const char* DIRDOWN;

const int MAX_PATH_LEN = 4;


void ReadFromZpar(std::string path,int DocId,ofstream& out);

std::string DIRT_X_Y(ZparTree ztree);

std::vector<string> DIRT_From_Zpar(ZparTree ztree);

void write_triple(std::vector<std::string>triple,ofstream& out);

void write_quadruple(std::vector<std::string>triple,ofstream& out,int sentId);

bool is_Entity(int posid);

bool is_X(int wordid);

bool is_Y(int wordid);

bool  check_direction(int lca,std::vector<int>hs,bool isLeft,ZparTree ztree);

bool compare_position(int head,int modifier,ZparTree ztree);

std::vector<Path> get_satellite_links(Path p,ZparTree ztree);

std::string clean_path(Path p1,ZparTree ztree);

std::string edge_to_string(int id,ZparTree ztree,bool isHead=false);

std::string argument_to_string(int id,std::string edge_name,ZparTree ztree,bool isHead);




