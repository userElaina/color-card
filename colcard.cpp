#include "colcard.hpp"
#include "cmdline/cmdline.h"

int main(int argc,char **argv){
    cmdline::parser a;
    a.add<std::string>("function",'f',"function name",true,"",cmdline::oneof<std::string>("crowd","naive","line","linear"));

    a.add("help", 'h', "print this help message");
    a.set_program_name("colcard");
    a.parse(argc,argv);


}