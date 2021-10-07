// code by userElaina
#include "bmp24bits.hpp"

#define DEBUG 0
int LOG=0xffffff;

#define ERR_INPUT_SIZE_1 (-17)

#define Q_MAX 256
class Q{
public:
	int num[8][Q_MAX],col[8][Q_MAX];
	const int total,limit,need;
	const std::string pth;

	Q(std::string s,int t,double l,int n=Q_MAX):pth(s),total(t),limit(l*(double)t+0.5),need(n<Q_MAX?n:Q_MAX){
		memset(num,0,8);
		memset(col,0,8);
	}
	const int search(int w,int n){
		if(n<limit)return need;
		for(int i=0;i<need;i++)
			if(n>num[w][i])return i;
		return need;
	}
	int insert(int w,int n,int c){
		w=w-1;
		const int s=search(w,n);
		if(s==need)
			return 1;
		for(int i=need-1;i>s;i--){
			num[w][i]=num[w][i-1];
			col[w][i]=col[w][i-1];
		}
		num[w][s]=n;
		col[w][s]=c;
		return 0;
	}
	void show(){
		for(int w=0;w<8;w++)for(int i=0;i<need;i++){
			if(num[w][i]<=limit)break;
			printf("%d %06X %d\n",w+1,col[w][i],num[w][i]);
		}
	}
	void draw(){
		FILE*f=fopen((pth+".html").c_str(),"w");
		std::string s=pth.substr(pth.find_last_of("/\\")+1);
		s=s.substr(0,s.find_last_of("."));
		s=s.substr(0,s.find_last_of("."));
		fprintf(f,"<!DOCTYPE html>\n<html>\n\t<head>\n\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n\t\t<title>\n\t\t\tColor Card\n\t\t</title>\n\t</head>\n\t<style>\n\t\tbody{\n\t\t\tbackground-image:url(\"%s\");\n\t\t\tbackground-size:100%s auto;\n\t\t}\n\t</style>\n\t<body style=\"font-family:Consolas;text-align:center;\"><h1>Color Card</h1>\n",s.c_str(),"%");
		for(int w=0;w<8;w++){
			if(num[w][0]<=limit)continue;
			fprintf(f,"\t\t<p>\n\t\t\t<div style=\"font-size:24px;font-weight:bold;\">Level %d</div>\n",w+1);
			for(int i=0;i<need;i++){
				if(num[w][i]<=limit)break;
				fprintf(f,"\t\t\t<div style=\"color:#%06X;background-color:#%06X;\">#%06X&nbsp;",col[w][i]^0x808080,col[w][i],col[w][i]);
				for(int k=1000000000;k>1;k/=10){
					if(k>num[w][i])fprintf(f,"&nbsp;");
					else break;
				}
				double d=num[w][i]*100.0/total;
				fprintf(f,"%dpixels&nbsp;(%s%.3lf%)</div>\n",num[w][i],d<10.0?"&nbsp;":"",d);
			}
			fprintf(f,"\t\t</p>\n");
		}
		fprintf(f,"\t</body>\n</html>\n");
	}
};

inline void crowd(char*pth,double limit=0.01,int need=Q_MAX){
	if(LOG&1)printf("Loading...\n");
	std::string s=bmp(pth);
	BMP24bits*p=new BMP24bits(s);

	if(LOG&1)printf("Initializing...\n");
	int*inverse=(int*)malloc(1<<29);//1<<1<<24<<2<<2
	memset(inverse,0,8);
	
	if(LOG&1)printf("Analyzing...\n");
	for(int i=0;i<p->size;){
		register int b=p->o[i++];
		register int g=p->o[i++];
		register int r=p->o[i++];
		inverse[(1<<24|r<<16|g<<8|b)<<2]++;
	}

	if(LOG&1)printf("Sorting...\n");
	Q*q=new Q(s,p->size/3,limit,need);
	free(p);
	for(int w=8;w>0;w--){
		const int _get_tail=(1<<w)-1;
		const int _max=1<<(w+(w<<1)+2);
		const int _split[3]{2,w+2,(w+1)<<1};
		const int _decompress[3]{8-w,16-w,24-w};
		const int _compress[3]{2,w+1,w<<1};
		for(int i=_max;i<(_max<<1);i+=4){

			const int bgr[3]={
				(i>>_split[0])&_get_tail,
				(i>>_split[1])&_get_tail,
				(i>>_split[2])&_get_tail
			};

			const int x=inverse[i];
			const int ext[3]={inverse[i|1],inverse[i|2],inverse[i|3]};

			const int col=bgr[2]<<_decompress[2]|bgr[1]<<_decompress[1]|bgr[0]<<_decompress[0];
			const int avg=x?((ext[2]+(x>>1))/x)<<16|((ext[1]+(x>>1))/x)<<8|((ext[0]+(x>>1))/x):0;
			q->insert(w,x,col+avg);
			int j=_max>>3|(bgr[2]>>1)<<_compress[2]|(bgr[1]>>1)<<_compress[1]|(bgr[0]>>1)<<_compress[0];
			inverse[j++]+=x;
			inverse[j++]+=ext[0]+((x*(bgr[0]&1))<<_decompress[0]);
			inverse[j++]+=ext[1]+((x*(bgr[1]&1))<<_decompress[0]);
			inverse[j++]+=ext[2]+((x*(bgr[2]&1))<<_decompress[0]);
		}
	}

	if(q->total^inverse[4])printf("Warning: Checksum failure.");
	free(inverse);

	if(LOG&1)printf("Drawing...\n");
	if(LOG&2)q->show();
	if(LOG&4)q->draw();
	free(q);

	if(LOG&1)printf("Ending...\n");
}

inline void naive(char*pth,int needx=4,int needy=4){
	if(LOG&1)printf("Loading...\n");
	const int need=needx*needy;
	if(0>=need||need>=Q_MAX)err(ERR_INPUT_SIZE_1);
	std::string s=bmp(pth,needx,needy);
	BMP24bits*p=new BMP24bits(s);

	if(LOG&1)printf("Analyzing...\n");
	Q*q=new Q(s,need,0,need);
	for(int i=0;i<p->size;){
		register int b=p->o[i++];
		register int g=p->o[i++];
		register int r=p->o[i++];
		q->insert(1,1,r<<16|g<<8|b);
	}
	free(p);

	if(LOG&1)printf("Drawing...\n");
	if(LOG&2)q->show();
	if(LOG&4)q->draw();
	free(q);

	if(LOG&1)printf("Ending...\n");
}
