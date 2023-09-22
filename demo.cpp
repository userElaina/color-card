#include "colcard.hpp"

int main(int argc,char **argv){
    OUTPUT=OUTPUT_DEBUG|OUTPUT_PROGRESS|OUTPUT_LIST|OUTPUT_HTML|OUTPUT_PIC;
    try{
        crowd("./example/100154449_p0.jpg");
        crowd("./example/87011701_p0.jpg");
        naive("./example/87011701_p0.jpg");
        naive("./example/87011701_p0.jpg",7,8);
        line("./example/87011701_p0.jpg",192);
        line("./example/87011701_p0.jpg",96,1);
        line("./example/87011701_p0.jpg",96,0,0xe39dac);
        line("./example/douban.jpg",9,1);
        linear("./example/kuaiche.jpg",0xfffffe,0xffffff,0x000000,0xffffff);
    }catch(const char *s){
        throw s;
    }
}
