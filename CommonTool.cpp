//
// Created by wpf on 8/30/16.
//

#include <codecvt>
#include <locale>
#include <stdlib.h>
#include <sstream>
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include "CommonTool.h"


std::wstring CommonTool::s2ws(const std::string &str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

std::string CommonTool::ws2s(const std::wstring &wstr) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

//stanford sentenceId 从1开始
void CommonTool::getInfofromStanford(std::string xmlpath, std::vector<Token> &nerTokens,
                                     std::vector<Corefer> &corefers,int DocId) {

    rapidxml::file<> fdoc(xmlpath.c_str());

    rapidxml::xml_document<> doc;

    doc.parse<0>(fdoc.data());


    rapidxml::xml_node<>* root = doc.first_node();

    rapidxml::xml_node<>* document = root->first_node();

    rapidxml::xml_node<>* sentences = document->first_node("sentences");

    rapidxml::xml_node<>* coreferences = document->first_node("coreference");

    for(auto node = sentences->first_node("sentence");node!= nullptr;node = node->next_sibling()){

        rapidxml::xml_node<>* tokens = node->first_node("tokens");

        for(auto node_token= tokens->first_node("token");node_token!= nullptr;node_token = node_token->next_sibling()){

            Token token;

            token.sentenceid = atoi(node->first_attribute("id")->value())-1; // Begin from 0
            token.tokenid = atoi(node_token->first_attribute("id")->value())-1;  // Change or not
            token.word = node_token->first_node("word")->value();
            token.ner = node_token->first_node("NER")->value();

            token.Documentid = DocId;

            nerTokens.push_back(token);

        }
    }

    int cosetid=1;
    for(auto node = coreferences->first_node("coreference");node!= nullptr;node=node->next_sibling(),cosetid++){

        for(auto mention_node = node->first_node("mention");mention_node!= nullptr;mention_node=mention_node->next_sibling()){

            Corefer corefer;

            auto res = mention_node->first_attribute("representative");

            if(res!= nullptr){
                corefer.isRepresentative = true;
                //std::cout<<res->value()<<std::endl;
            }else{
                corefer.isRepresentative = false;
            }

            corefer.sentenceid = atoi(mention_node->first_node("sentence")->value())-1; //As above
            corefer.Cosetid = cosetid;
            corefer.startIndex = atoi(mention_node->first_node("start")->value());
            corefer.endIndex = atoi(mention_node->first_node("end")->value());
            corefer.headIndex = atoi(mention_node->first_node("head")->value());
            corefer.text = mention_node->first_node("text")->value();

            corefer.Documentid = DocId;

            corefers.push_back(corefer);

        }

    }

}

int CommonTool::lcs(std::wstring A, std::wstring B) {

    std::vector<std::vector<int> > len;
    len.resize(A.size() + 1);
    for (int i = 0; i <= A.size(); i++) {
        len[i].resize(B.size() + 1, 0);
    }
    for (int i = 1; i <= A.size(); ++i) {
        for (int j = 1; j <= B.size(); ++j) {
            if (A[i - 1] == B[j - 1]) {
                len[i][j] = len[i - 1][j - 1] + 1;
            } else if (len[i - 1][j] >= len[i][j - 1]) {
                len[i][j] = len[i - 1][j];
            } else {
                len[i][j] = len[i][j - 1];
            }
        }
    }
    int apos = A.size();
    int bpos = B.size();
    int commonlen = len[apos][bpos];
    int k = commonlen;
    /*common.resize(commonlen);
    while(apos && bpos){
        if(len[apos][bpos] == len[apos-1][bpos] + 1){
            common[--k] = A[--apos];
            --bpos;
        }else if (len[apos-1][bpos] >= len[apos][bpos-1])
        {
            --apos;
        }else{
            --bpos;
        }
    }
    for(int i = 0;i < commonlen;i++){
        cout<<common[i];
    }
    cout<<endl;*/
    return commonlen;
}

