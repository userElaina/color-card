// code by userElaina
#include<bits/stdc++.h>

#define LL long long
#define ULL unsigned long long
#define B unsigned char
#define pt putchar
#define gt getchar
#define register
// #define inline

#define WARN_EXTRA_DATA 1
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

#define size_head 0x36
#define DEFAULT_Y -2147483647
#define DEFAULT_Z -2147483646
#define DEFAULT_G -2147483643
#define DEFAULT_B -2147483642

inline int argb(int r,int g=DEFAULT_G,int b=DEFAULT_B,int a=0){
	if(a<0||a>0xff)
		return ERR_NOT_24BIT_2;
	if(g==DEFAULT_G&&b==DEFAULT_B){
		if(r<0||r>0xffffff)
			return ERR_NOT_24BIT_1;
		if(!a)
			return r;
		b=r&0xff;
		g=(r>>8)&0xff;
		r=r>>16;
	}else{
		if(r<0||r>0xff||g<0||g>0xff||b<0||b>0xff)
			return ERR_NOT_24BIT_1;
	}
	if(a){
		r+=a-(r*a+127)/255;
		g+=a-(g*a+127)/255;
		b+=a-(b*a+127)/255;
	}
	return r<<16|g<<8|b;
}

inline std::string bmp(std::string pth,int resizex=0,int resizey=0,std::string ext="original"){
	std::string rx=std::to_string(resizex),ry=std::to_string(resizey);
	std::string p2=pth+"."+((resizex|resizey)?rx+"x"+ry:ext)+".bmp";
	std::string s="ffmpeg -y -i \""+pth+"\" "+((resizex|resizey)?"-vf scale="+rx+":"+ry+" ":"")+"\""+p2+"\" 1>nul 2>&1";
	system(s.c_str());

	return p2;
}

inline std::string bmp(std::string pth,std::string ext){
	return bmp(pth,0,0,ext);
}

class BMP24bits{
private:
	int size_o;
	B*p;
	inline int _nc(){return *p++;}
	inline int _int4(){return _nc()|_nc()<<8|_nc()<<16|_nc()<<24;}
	inline void _sethead(LL reg,int l,int r){
		for(int i=l;i<=r;i++){
			head[i]=reg&0xff;
			reg>>=8;
		}
	}
	inline int _getxy(int x,int y=DEFAULT_Y){
		if(y==DEFAULT_Y){
			if(x<0||x>=size)
				return ERR_SIZE_3;
		}else{
			if(x<0||y<0||x>=width||y>=height)
				return ERR_SIZE_3;
			x=x+y*width;
		}
		return x;
	}

public:
	int size,width,height,code;
	B*o;
	int*tag;
	int lastag=0;
	B head[size_head]={
		0x42,0x4d,0xff,0xff,	0xff,0xff,0x00,0x00,	0x00,0x00,0x36,0x00,	0x00,0x00,0x28,0x00,
		0x00,0x00,0xff,0xff,	0xff,0xff,0xff,0xff,	0xff,0xff,0x01,0x00,	0x18,0x00,0x00,0x00,
		0x00,0x00,0xff,0xff,	0xff,0xff,0x00,0x00,	0x00,0x00,0x00,0x00,	0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,	0x00,0x00
	};

	BMP24bits(LL x,LL y){
		width=x;
		height=y;

		size=width*height;
		size_o=size+(size<<1);

		const LL size_data=(width+(width<<1)+(width&3))*height;
		const LL xy=y<<(LL)32|x;
		const LL size_file=size_data+size_head;

		_sethead(size_file,0x02,0x05);
		_sethead(xy,0x12,0x19);
		_sethead(size_data,0x22,0x25);

		alloc();
		white();
	}

	BMP24bits(std::string p){
		code=read(p);
	}
	
	inline void alloc(){
		o=(B*)malloc(size_o);
		tag=(int*)malloc(size<<2);
		memset(tag,0,size<<2);
	}
	inline void white(){memset(o,0xff,size_o);}
	inline void black(){memset(o,0x00,size_o);}

	inline int read(std::string pth){
		FILE*f=fopen(pth.c_str(),"rb");
		if(fread(head,1,0x36,f)^0x36)return ERR_SIZE_1;
		p=head;

		if(_nc()^'B')return ERR_NOT_BMP_1;
		if(_nc()^'M')return ERR_NOT_BMP_2;
		// const "BM"

		const int size_file=_int4();

		if(_int4())return ERR_OTHER_DATA_1;
		// const 0x00

		if(_int4()^size_head)return ERR_NOT_24BIT_1;
		// head size 0x36 bits
		
		if(_int4()^0x28)return ERR_NOT_24BIT_1;
		// head 2 size 0x36 bits

		width=_int4();
		height=_int4();

		size=width*height;
		size_o=size+(size<<1);

		const int width3=width+(width<<1);
		const int mo=width&3;

		if(size_file^((width3+mo)*height+0x36))return ERR_NOT_24BIT_2;
		// 4B

		if(_int4()^0x180001)return ERR_OTHER_DATA_2;
		// const 0x00180001

		if(_int4())return ERR_NOT_BMP_2;
		// const compress 0x00

		if(size_file^(_int4()+0x36))return ERR_NOT_24BIT_3;
		// img_size and file_size

		_int4();
		_int4();
		// paint=0x1d87 ffmpeg=0x0000 
		if(_int4())return ERR_OTHER_DATA_3;
		// const

		p=(B*)malloc(4);
		alloc();
		for(int i=0;i<height;i++){
			if(fread(o+i*width3,1,width3,f)^width3)return ERR_SIZE_2;
			if(fread(p,1,mo,f)^mo)return ERR_SIZE_2;
		}

		if(fread(p,1,1,f))
			return 1;
		return 0;
	}

	inline int save(std::string pth,int compress=0){
		FILE*f=fopen((compress?pth+".bmp":pth).c_str(),"wb");
		const int width3=width+(width<<1);
		const int mo=width&3;
		p=(B*)malloc(4);
		memset(p,0,4);
		fwrite(head,1,0x36,f);
		for(int i=0;i<height;i++){
			fwrite(o+i*width3,1,width3,f);
			fwrite(p,1,mo,f);
		}
		fclose(f);
		if(!compress)return 0;
		
		std::string s="ffmpeg -y -f bmp_pipe -i \""+pth+".bmp\" \""+pth+"\" 1>nul 2>&1";
		system(s.c_str());
		return 0;
	}

	inline int getag(int x,int y=DEFAULT_Y){
		if((x=_getxy(x,y))<0)
			return x;
		return tag[x];
	}

	inline int getpixel(int x,int y=DEFAULT_Y){
		if((x=_getxy(x,y))<0)
			return x;
		x=x+(x<<1);
		register int b=o[x++];
		register int g=o[x++];
		register int r=o[x];
		return r<<16|g<<8|b;
	}

	inline int setag(int x,int y=DEFAULT_Y,int z=DEFAULT_Z){
		if((x=_getxy(x,y))<0)
			return x;
		if(z^DEFAULT_Z)lastag=z;
		tag[x]=lastag;
		return 0;
	}

	inline int setpixel(int x,int y=DEFAULT_Y,int r=0x000000,int g=DEFAULT_G,int b=DEFAULT_B,int a=0){
		if((x=_getxy(x,y))<0)
			return x;
		x=x+(x<<1);
		if((r=argb(r,g,b,a))<0)
			return r;
		o[x++]=r&0xff;
		r>>=8;
		o[x++]=r&0xff;
		r>>=8;
		o[x]=r;
		return 0;
	}

	inline int bgray(int r=0xffffff,int g=DEFAULT_G,int b=DEFAULT_B,int a=0){
		if((r=argb(r,g,b,a))<0)
			return r;
		b=r&0xff;
		g=(r>>=8)&0xff;
		r>>=8;
		for(int i=0;i<size_o;){
			const int c=299*o[i+2]+587*o[i+1]+114*o[i];
			o[i++]=(c*b+127500)/255000;
			o[i++]=(c*g+127500)/255000;
			o[i++]=(c*r+127500)/255000;
		}
		return 0;
	}

	inline int wgray(int r=0x000000,int g=DEFAULT_G,int b=DEFAULT_B,int a=0){
		if((r=argb(r,g,b,a))<0)
			return r;
		b=(r&0xff)^0xff;
		g=((r>>=8)&0xff)^0xff;
		r=(r>>8)^0xff;
		for(int i=0;i<size_o;){
			const int c=255000-(299*o[i+2]+587*o[i+1]+114*o[i]);
			o[i++]=((c*b+127500)/255000)^0xff;
			o[i++]=((c*g+127500)/255000)^0xff;
			o[i++]=((c*r+127500)/255000)^0xff;
		}
		return 0;
	}
};

inline int distance(int l,int r){
	if(l<0||l>0xffffff||r<0||r>0xffffff)
		return ERR_NOT_24BIT_1;
	l^=r;
	return (l>>16)+((l>>8)&0xff)+(l&0xff);
}
