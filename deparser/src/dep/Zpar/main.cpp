//
// Created by bruce on 16-3-15.
//

#include "Zpar.h"
#include <iostream>
#include <fstream>

int main(int argc,char** argv){


    Zpar zp("/home/wpf/zpar-0.7.5/chinese-models");

    std::string sent= "今天天气如何";
    std::string aaa=zp.depparser(sent);

    std::cout<<aaa;


    return 0;
}
