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

typedef boost::bimap<std::string, int> bimap;

int start_wid=2;
int start_did=1;


bimap  word2id;

bimap  dep2id;

bimap pos2id = boost::assign::list_of< bimap::relation >
        ("AD",1)
("AS",2)
("BA",3)
("CC",4)
("CD",5)
("CS",6)
("DEC",7)
("DEG",8)
("DER",9)
("DEV",10)
("DT",11)
("ETC",12)
("FW",13)
("IJ",14)
("JJ",15)
("LB",16)
("LC",17)
("M",18)
("MSP",19)
("NN",20)
("NR",21)
("NT",22)
("OD",23)
("ON",24)
("P",25)
("PN",26)
("PU",27)
("SB",28)
("SP",29)
("VA",30)
("VC",31)
("VE",32)
("VV",33);
