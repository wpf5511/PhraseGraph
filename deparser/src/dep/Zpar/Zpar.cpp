#include "Zpar.h"
#include <fstream>
#include <stdlib.h>
#include <string>

Zpar::Zpar(){}

Zpar::~Zpar(){
    (*Par).~CDepParser();
}


Zpar::Zpar(const std::string &modeldir){

    std::string sTaggerFeatureFile = modeldir + "/tagger";
    std::string sParserFeatureFile = modeldir + "/depparser";

    tagger = new CTagger(sTaggerFeatureFile, false, 1024, false);

    Par=new CDepParser(sParserFeatureFile, false, false);

}


//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> Zpar::converter;

 std::wstring_convert<std::codecvt_utf8<char32_t>,char32_t> Zpar::converter;



 std::string Zpar::To_UTF8(const std::u32string& from){

    return Zpar::converter.to_bytes(from);
}


 std::u32string Zpar::To_UTF32(const std::string &from) {

    return Zpar::converter.from_bytes(from);

}





std::string Zpar::depparser(std::string &sentence){

    std::wcout.imbue(std::locale(""));

    std::u32string wsent = To_UTF32(sentence);

    CStringVector *input_sent = new CStringVector;

    std::for_each(wsent.begin(),wsent.end(),[&input_sent](char32_t character){input_sent->push_back(Zpar::To_UTF8(std::u32string(1,character)));});

    CTwoStringVector *tagged_sent = new CTwoStringVector;

    tagger->tag(input_sent, tagged_sent, NULL, 1);

    std::ostringstream ans;

    CLabeledDependencyTree *output_sent = new CLabeledDependencyTree;


    Par->parse(*tagged_sent, output_sent);


    ans<<*output_sent;

    delete  input_sent;
    delete  output_sent ;

    return ans.str();

}
