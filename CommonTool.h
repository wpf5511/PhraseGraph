//
// Created by wpf on 8/30/16.
//

#ifndef PHRASEGRAPH_COMMONTOOL_H
#define PHRASEGRAPH_COMMONTOOL_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

struct Token{
    int Documentid;
    int sentenceid;
    int tokenid;
    std::string word;
    std::string ner;
};

struct Corefer{
    int Documentid;
    int sentenceid;
    int Cosetid;
    bool isRepresentative;
    int startIndex;
    int endIndex;
    int headIndex;
    std::string text;

};

struct NamedEntityPhrase{
    std::string headstring;
    std::string phrase;
    std::string entityType;
};

typedef std::map<int,NamedEntityPhrase> NEPMAP;  //phraseid to ner



class CommonTool {
public:
    static std::wstring s2ws(const std::string& str);

    static std::string ws2s(const std::wstring& wstr);

    static void getInfofromStanford(std::string xmlpath,std::vector<Token> &nerTokens,std::vector<Corefer> &corefers,int DocId);

    static int lcs(std::wstring A,std::wstring B);


    };



#endif //PHRASEGRAPH_COMMONTOOL_H
