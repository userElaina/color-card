// code by userElaina
#include<bits/stdc++.h>

#define LL long long
#define ULL unsigned long long
#define B unsigned char
#define pt putchar
#define gt getchar
#define register
// #define inline

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

inline std::string bmp(const char*pth,int resizex=0,int resizey=0){
	std::string rx=std::to_string(resizex),ry=std::to_string(resizey);
	std::string p2=pth;
	p2+=".";
	p2+=(resizex|resizey)?rx+"x"+ry:"original";
	p2+=".bmp";
	
	std::string s="ffmpeg -y -i \"";
	s+=pth;
	s+="\" ";
	if(resizex|resizey)
		s+="-vf scale="+rx+":"+ry+" ";
	s+="\""+p2+"\" 1>nul 2>&1";
	system(s.c_str());

	return p2;
}

inline void err(int code){
	printf("Error code: %d",code);
	exit(0);
}

class BMP24bits{
private:
	B*p;
	inline int _nc(){return *p++;}
	inline int _int4(){return _nc()|_nc()<<8|_nc()<<16|_nc()<<24;}
	inline void _sethead(LL reg,int l,int r){
		for(int i=l;i<=r;i++){
			head[i]=reg&0xff;
			reg>>=8;
		}
	}

public:
	B*o;
	B head[0x36]={
		0x42,0x4d,0xff,0xff,	0xff,0xff,0x00,0x00,	0x00,0x00,0x36,0x00,	0x00,0x00,0x28,0x00,
		0x00,0x00,0xff,0xff,	0xff,0xff,0xff,0xff,	0xff,0xff,0x01,0x00,	0x18,0x00,0x00,0x00,
		0x00,0x00,0xff,0xff,	0xff,0xff,0x00,0x00,	0x00,0x00,0x00,0x00,	0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,	0x00,0x00
	};

	int size,width,height,code;
	BMP24bits(LL x,LL y){
		width=x;
		height=y;
		_sethead(y<<32|x,0x12,0x19);

		size=x*y;
		size+=size<<1;
		_sethead(size,0x22,0x25);

		_sethead(size+0x36,0x02,0x05);

		o=(B*)malloc(size);
		clear();
	}

	BMP24bits(std::string p){
		code=read(p);
		if(code)
			err(code);
	}
	
	inline int clear(){memset(o,0xff,8);}

	inline int read(const std::string pth){
		FILE*f=fopen(pth.c_str(),"rb");
		if(fread(head,1,0x36,f)^0x36)return ERR_SIZE_1;
		p=head;

		if(_nc()^'B')return ERR_NOT_BMP_1;
		if(_nc()^'M')return ERR_NOT_BMP_2;
		// const "BM"

		register int file_size=_int4();

		if(_int4())return ERR_OTHER_DATA_1;
		// const 0x00

		if(_int4()^0x36)return ERR_NOT_24BIT_1;
		// head size 0x36 bits
		
		if(_int4()^0x28)return ERR_NOT_24BIT_1;
		// head 2 size 0x36 bits

		width=_int4();height=_int4();
		int width3=width+(width<<1);
		size=width3*height;
		int mo=width&3;
		int width3mo=width3+mo;

		if(file_size^(width3mo*height+0x36))return ERR_NOT_24BIT_2;
		// 4B

		if(_int4()^0x180001)return ERR_OTHER_DATA_2;
		// const 0x00180001

		if(_int4())return ERR_NOT_BMP_2;
		// const compress 0x00

		if((_int4()+0x36)^file_size)return ERR_NOT_24BIT_3;
		// img_size and file_size

		if(_int4())return ERR_OTHER_DATA_3;
		if(_int4())return ERR_OTHER_DATA_3;
		if(_int4())return ERR_OTHER_DATA_3;
		// const 0x000000

		p=(B*)malloc(4);
		o=(B*)malloc(size);
		for(int i=0;i<height;i++){
			if(fread(o+i*width3,1,width3,f)^width3)return ERR_SIZE_2;
			if(fread(p,1,mo,f)^mo)return ERR_SIZE_2;
		}

		if(fread(p,1,1,f))
			return 1;

		return 0;
	}

	inline int save(const std::string pth){
		FILE*f=fopen(pth.c_str(),"wb");
		int width3=width+(width<<1),mo=width&3;
		p=(B*)malloc(4);
		memset(p,0,8);
		fwrite(head,1,0x36,f);
		for(int i=0;i<height;i++){
			fwrite(o+i*width3,1,width3,f);
			fwrite(p,1,mo,f);
		}
		return 0;
	}
};

