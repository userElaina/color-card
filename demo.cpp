#include "color_card.hpp"

int main(int argc,char **argv){
	LOG=0xffffff;
	crowd("./example/87011701_p0.jpg");
	printf("\n");
	naive("./example/87011701_p0.jpg");
	naive("./example/87011701_p0.jpg",7,8);
}
