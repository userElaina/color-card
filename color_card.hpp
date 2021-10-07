// code by userElaina
#include "bmp24bits.hpp"

#define OUTPUT_DEBUG 1
#define OUTPUT_PROGRESS 2
#define OUTPUT_LIST 4
#define OUTPUT_HTML 8
#define OUTPUT_PIC 16
int OUTPUT=0xffffff;

#define WARN_CHECKSUM 2
#define ERR_INPUT_SIZE_1 (-17)

inline void check_code(int code){
	if(code){
		if(code<0){
			printf("Error code: %d\n",code);
			exit(0);
		}else{
			printf("Warning code: %d\n",code);
		}
	}
}

#define Q_MAX 256

class Q{
public:
	int num[8][Q_MAX],col[8][Q_MAX];
	const int total,limit,need;
	const std::string pth;

	Q(std::string s,int t,double l,int n=Q_MAX):pth(s),total(t),limit(l*(double)t+0.5),need(n<Q_MAX?n:Q_MAX){
		memset(num,0,Q_MAX<<5);
		memset(col,0,Q_MAX<<5);
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
		FILE*f=fopen((pth.substr(0,pth.find_last_of("."))+".html").c_str(),"w");
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

inline void crowd(std::string pth,double limit=0.01,int need=Q_MAX){
	if(OUTPUT&OUTPUT_PROGRESS)printf("Loading...\n");
	std::string s=bmp(pth,"crowd");
	BMP24bits*p=new BMP24bits(s);
	check_code(p->code);

	if(OUTPUT&OUTPUT_PROGRESS)printf("Initializing...\n");
	int*inverse=(int*)malloc(1<<29);//1<<1<<24<<2<<2
	memset(inverse,0,1<<29);
	
	if(OUTPUT&OUTPUT_PROGRESS)printf("Analyzing...\n");
	for(int i=0;i<p->size;i++){
		inverse[(1<<24|p->getpixel(i))<<2]++;
	}

	if(OUTPUT&OUTPUT_PROGRESS)printf("Sorting...\n");
	Q*q=new Q(s,p->size,limit,need);
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

	if(q->total^inverse[4])check_code(WARN_CHECKSUM);
	free(inverse);
	if(OUTPUT&OUTPUT_LIST)q->show();

	if(OUTPUT&OUTPUT_PROGRESS)printf("Saving...\n");
	if(OUTPUT&OUTPUT_HTML)q->draw();
	free(q);

	if(OUTPUT&OUTPUT_PROGRESS)printf("Ending...\n\n");
}

inline void naive(std::string pth,int needx=4,int needy=4){
	if(OUTPUT&OUTPUT_PROGRESS)printf("Loading...\n");
	const int need=needx*needy;
	if(0>=need||need>=Q_MAX)check_code(ERR_INPUT_SIZE_1);
	std::string s=bmp(pth,needx,needy);
	BMP24bits*p=new BMP24bits(s);
	check_code(p->code);

	if(OUTPUT&OUTPUT_PROGRESS)printf("Analyzing...\n");
	Q*q=new Q(s,need,0,need);
	for(int i=0;i<p->size;i++){
		q->insert(1,1,p->getpixel(i));
	}
	free(p);

	if(OUTPUT&OUTPUT_LIST)q->show();
	if(OUTPUT&OUTPUT_PROGRESS)printf("Saving...\n");
	if(OUTPUT&OUTPUT_HTML)q->draw();
	free(q);

	if(OUTPUT&OUTPUT_PROGRESS)printf("Ending...\n\n");
}

inline void line(std::string pth,int dis=96,int onlyline=0,int color=-1){
	if(OUTPUT&OUTPUT_PROGRESS)printf("Loading...\n");
	std::string s=bmp(pth);
	BMP24bits*p=new BMP24bits(s);
	check_code(p->code);

	if(dis>=765){
		if(color>0)p->wgray(color);
		if(OUTPUT&OUTPUT_PROGRESS)printf("Saving...\n");
		if(OUTPUT&OUTPUT_PIC)p->save(pth+".765"+(onlyline?"_l":"")+".png",1);
		if(OUTPUT&OUTPUT_PROGRESS)printf("Ending...\n\n");
		return;
	}

	if(OUTPUT&OUTPUT_PROGRESS)printf("Initializing...\n");
	int*bfsx=(int*)malloc(p->size<<2);
	int*bfsy=(int*)malloc(p->size<<2);
	int*bfsz=(int*)malloc(p->size<<2);
	int t=0,cls=0;

	if(OUTPUT&OUTPUT_PROGRESS)printf("Analyzing...\n");
	const int delta_x[4]={1,-1,0,0},delta_y[4]={0,0,1,-1};
	for(int i=0;i<p->height;i++){
		for(int j=0;j<p->width;j++){
			if(p->getag(j,i))continue;
			p->setag(j,i,++cls);
			bfsx[t]=j;
			bfsy[t]=i;
			bfsz[t]=p->getpixel(j,i);
			t++;
			for(int k=t-1;k<t;k++){
				const int x=bfsx[k],y=bfsy[k],z=bfsz[k];
				for(int delta_z=0;delta_z<4;delta_z++){
					const int xx=x+delta_x[delta_z],yy=y+delta_y[delta_z];
					const int col=p->getpixel(xx,yy);
					if(col<0||distance(z,col)>dis)continue;
					if(OUTPUT&OUTPUT_DEBUG)if(xx<0||xx>=p->width||yy<0||yy>=p->height)
						printf("%d %d\n",xx,yy);
					if(p->getag(xx,yy))continue;
					p->setag(xx,yy,cls);
					bfsx[t]=xx;
					bfsy[t]=yy;
					bfsz[t]=col;
					t++;
				}
			}
		}
	}
	
	if(OUTPUT&OUTPUT_PROGRESS)printf("Drawing...\n");
	for(int i=0;i<p->height;i++){
		for(int j=0;j<p->width;j++){
			int flg=0;
			const int tg=p->getag(j,i);
			for(int delta_z=0;delta_z<4;delta_z++){
				const int tg2=p->getag(j+delta_x[delta_z],i+delta_y[delta_z]);
				if(tg2<0)continue;
				if(tg^tg2)flg++;
			}
			if(flg){
				p->setpixel(j,i);
			}else if(onlyline){
				p->setpixel(j,i,0xffffff);
			}
		}
	}
	if(color>0)p->wgray(color);

	if(OUTPUT&OUTPUT_PROGRESS)printf("Saving...\n");
	if(OUTPUT&OUTPUT_PIC)p->save(pth+"."+std::to_string(dis)+(onlyline?"_l":"")+".png",1);

	if(OUTPUT&OUTPUT_PROGRESS)printf("Ending...\n\n");
}