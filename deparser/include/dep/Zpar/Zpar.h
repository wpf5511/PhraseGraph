#ifndef _ZPAR_H
#define _ZPAR_H

#include <string>
#include <vector>
#include <utility>
#include <locale>
#include <codecvt>

#include "definitions.h"
#include "depparser.h"
#include "tagger.h"

#include "reader.h"
#include "writer.h"
#include "stdlib.h"

using namespace TARGET_LANGUAGE;

struct Dependency_TreeNode{
    std::string word;
    std::string tag;
    std::string label;
    int head;
};



    class Zpar{
    public:
        Zpar();
        ~Zpar();
        Zpar(const std::string &modeldir);
        std::string depparser(std::string &sentence);

        //static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

        static std::wstring_convert<std::codecvt_utf8<char32_t>,char32_t> converter;



    private:

        CDepParser *Par;
        CTagger *tagger;

        static std::string To_UTF8(const std::u32string& from);

        static std::u32string To_UTF32(const std::string& from);

    };

#endif
