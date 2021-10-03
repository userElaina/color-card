// code by userElaina
#include<bits/stdc++.h>

#define LL long long
#define B unsigned char
#define pt putchar
#define gt getchar

#define register
#define inline

#define ERR_NOT_BMP_1 (-1)
#define ERR_NOT_BMP_2 (-2)
#define ERR_NOT_BMP_3 (-3)
#define ERR_NOT_24BIT_1 (-5)
#define ERR_NOT_24BIT_2 (-6)
#define ERR_NOT_24BIT_3 (-7)
#define ERR_OTHER_DATA_1 (-9)
#define ERR_OTHER_DATA_2 (-10)
#define ERR_OTHER_DATA_3 (-11)
#define ERR_SIZE_1 (-13)
#define ERR_SIZE_2 (-14)
#define ERR_SIZE_3 (-15)

class Rbmp24bits{
private:
	B*p;
	inline int test(const char*pth){
		FILE*f=fopen(pth,"rb");
		B head[0x36];
		if(fread(head,1,0x36,f)^0x36)return ERR_SIZE_1;
		p=head;

		if(nc()^'B')return ERR_NOT_BMP_1;
		if(nc()^'M')return ERR_NOT_BMP_2;
		// const "BM"

		register int full_size=int4();

		if(int4())return ERR_OTHER_DATA_1;
		// const 0x00

		if(int4()^0x36)return ERR_NOT_24BIT_1;
		// head size 0x36 bits
		
		if(int4()^0x28)return ERR_NOT_24BIT_1;
		// head 2 size 0x36 bits

		weight=int4();hight=int4();
		int weight3=weight+(weight<<1);
		size=weight3*hight;
		int weight3p=(weight&3)+weight3;

		if(full_size^(weight3p*hight+0x36))return ERR_NOT_24BIT_2;
		// 4B

		if(int4()^0x180001)return ERR_OTHER_DATA_2;
		// const 0x00180001

		if(int4())return ERR_NOT_BMP_2;
		// const compress 0x00

		if((int4()+0x36)^full_size)return ERR_NOT_24BIT_3;
		// img_size and full_size

		if(int4())return ERR_OTHER_DATA_3;
		if(int4())return ERR_OTHER_DATA_3;
		if(int4())return ERR_OTHER_DATA_3;
		// const 0x000000

		for(int i=0;i<hight;i++){
			if(fread(o+i*weight3,1,weight3,f)^weight3)return ERR_SIZE_2;
			if(fread(head,1,1,f)^1)return ERR_SIZE_2;
		}

		if(fread(head,1,1,f))
			return 1;

		return 0;
	}
	inline B nc(){return *p++;}
	inline int int4(){return nc()|nc()<<8|nc()<<16|nc()<<24;}
	inline void err(int code){printf("Error code: %d",code);exit(0);}

public:
	B o[100000000];
	int size,weight,hight,code;
	Rbmp24bits(const char*pth){
		code=test(pth);
		if(code)
			err(code);
	}
};

int lower(int x){
	return 'A'<=x&&x<='Z'?x-'A'+'a':x;
}

inline std::string bmp(const char*pth,LL resize=0){
	int weight=resize>>32,hight=resize&INT_MAX;
	std::string ws=std::to_string(weight),hs=std::to_string(hight);
	std::string p2="";
	p2+=pth;
	p2+=".";
	p2+=resize?ws+"x"+hs:"original";
	p2+=".bmp";
	
	std::string s="dir \"";
	s+=p2;
	s+="\" 1>nul 2>&1";
	if(system(s.c_str()))
		s="ffmpeg -i \"";
		s+=pth;
		s+="\" ";
		if(resize){
			s+="-vf scale="+ws+":"+hs+" ";
		}
		s+="\"";
		s+=p2;
		s+="\" 1>nul 2>&1";
		system(s.c_str());

	return p2;
}

class l2{
public:
	int num,col;
	l2(){}
	l2(int n,int c):num(n),col(c){}
	void setnum(int n){num=n;}
	void setcol(int c){col=c;}
	void set(int n,int c){setnum(n),setcol(c);}
	bool operator<(const l2&r){return num>r.num;}
	void show(){printf("#%06X %d\n",col,num);}
};

inline void crowd(char*pth,int w=2,LL resize=0){
	Rbmp24bits*p=new Rbmp24bits(bmp(pth,resize).c_str());
	int _head_and=(1<<w)-1,_tail_and=(1<<(8-w))-1,_mid=1<<(7-w);
	int _mmid=_mid<<16|_mid<<8|_mid;

	int len_bucket=1<<(w+(w<<1));
	int sum[len_bucket][4];
	memset(sum,0,sizeof(sum));
	
	for(int i=0;i<p->size;){
		int b=p->o[i++];
		int g=p->o[i++];
		int r=p->o[i++];
		int k=b>>(8-w)|(g>>(8-w)<<w)|(r>>(8-w)<<(w<<1));
		sum[k][0]+=(b&_tail_and)-_mid;
		sum[k][1]+=(g&_tail_and)-_mid;
		sum[k][2]+=(r&_tail_and)-_mid;
		sum[k][3]++;
	}

	int dt=0;
	l2 ans[len_bucket];
	for(int k=0;k<len_bucket;k++){
		if(!sum[k][3]){
			continue;
		}
		// printf("%d [%d, %d, %d, %d]\n",k,sum[k][0],sum[k][1],sum[k][2],sum[k][3]);
		int b=(sum[k][0]+(sum[k][0]>=0?sum[k][3]>>1:(~(sum[k][3]>>1))+1))/sum[k][3]+((k&_head_and)<<(8-w))+_mid;
		int g=(sum[k][1]+(sum[k][1]>=0?sum[k][3]>>1:(~(sum[k][3]>>1))+1))/sum[k][3]+(((k>>w)&_head_and)<<(8-w))+_mid;
		int r=(sum[k][2]+(sum[k][2]>=0?sum[k][3]>>1:(~(sum[k][3]>>1))+1))/sum[k][3]+(((k>>(w<<1))&_head_and)<<(8-w))+_mid;
		ans[dt++].set(sum[k][3],(r<<16|g<<8|b));
	}

	std::sort(ans,ans+dt);

	int ss=0;
	for(int i=0;i<dt;i++){
		ans[i].show();
		ss+=ans[i].num;
	}
}
