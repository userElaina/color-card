#include "color_card.hpp"

int main(int argc,char **argv){
    OUTPUT=OUTPUT_DEBUG|OUTPUT_PROGRESS|OUTPUT_LIST|OUTPUT_HTML|OUTPUT_PIC;
    try{
        crowd("./example/87011701_p0.jpg");
        crowd("./example/1 1.bmp");
        naive("./example/87011701_p0.jpg");
        naive("./example/87011701_p0.jpg",7,8);
        line("./example/87011701_p0.jpg",192);
        line("./example/87011701_p0.jpg",96,1);
        line("./example/87011701_p0.jpg",96,0,0xe39dac);
        line("./example/douban.jpg",9,1);
        line("./example/kuaiche.jpg",27,1);
    }catch(const char *s){
        puts(s);
    }
}
