//
// Created by wpf on 9/14/16.
//

#include <sstream>
#include "PhraseTransform.h"

char PhraseTransform::FIELD_SEPARATOR ='|';

char PhraseTransform::OPEN_BRACE ='(';

char PhraseTransform::CLOSE_BRACE =')';


PhraseTransform::PhraseTransform(const ZparTree ztree){
    phrasetree = ztree;
    setStringRepresent();
}


std::string PhraseTransform::getStringRepresent() {

    if(!phrasestring.empty()){
        return phrasestring;
    } else{
        int rootid = phrasetree.root_id;
        return createStringRepresent(rootid);
    }
}

void PhraseTransform::setStringRepresent() {

    if(phrasestring.empty()){

        int rootid = phrasetree.root_id;
        phrasestring = createStringRepresent(rootid);
    }
}




ZparTree PhraseTransform::getTreeRepresent() {

    return  phrasetree;
}

std::string PhraseTransform::createStringRepresent(int nodeid) {
    std::string currentNodeString = "";

    auto currentNode = phrasetree.get_Node(nodeid);

    std::string word = currentNode.lexeme;

    std::string pos = currentNode.pos;

    std::string dep = currentNode.dependency;

    currentNodeString = word+FIELD_SEPARATOR+pos+FIELD_SEPARATOR+dep;

    std::ostringstream os;

    os<<OPEN_BRACE;
    os<<currentNodeString;

    auto children_id = phrasetree.get_children(nodeid);

    for(auto child_id:children_id){

        os<<createStringRepresent(child_id);
    }

    os<<CLOSE_BRACE;

    return os.str();
}

bool PhraseTransform::operator<(const PhraseTransform&other) const{
    return this->phrasestring<other.phrasestring;
}