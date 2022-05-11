// code by userElaina
#include "bmp/bmp.hpp"

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

    Q(int t,double l,int n=Q_MAX):total(t),limit(l*(double)t+0.5),need(n<Q_MAX?n:Q_MAX){
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
        for(int w=0;w<8;w++){
            if(num[w][0]>limit)
                printf("Level %d:\n",w+1);
            else
                break;
            for(int i=0;i<need;i++){
                if(num[w][i]<=limit)break;
                if(num[w][i]>1)printf("%06X %d\n",col[w][i],num[w][i]);
                else printf("%06X\n",col[w][i]);
            }
        }
    }
    void draw(std::string pth,std::string background=""){
        if(background.empty())background=pth;
        FILE*f=fopen((pth.substr(0,pth.find_last_of("."))+".html").c_str(),"w");
        fprintf(f,"<!DOCTYPE html>\n<html>\n    <head>\n        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n        <title>\n            Color Card\n        </title>\n    </head>\n    <style>\n        body{\n            background-image:url(\"%s\");\n            background-size:100%s auto;\n        }\n    </style>\n    <body style=\"font-family:Consolas;text-align:center;\"><h1>Color Card</h1>\n",background.substr(pth.find_last_of("/\\")+1).c_str(),"%");
        for(int w=0;w<8;w++){
            if(num[w][0]<=limit)continue;
            fprintf(f,"        <p>\n            <div style=\"font-size:24px;font-weight:bold;\">Level %d</div>\n",w+1);
            for(int i=0;i<need;i++){
                if(num[w][i]<=limit)break;
                fprintf(f,"            <div style=\"color:#%06X;background-color:#%06X;\">#%06X&nbsp;",col[w][i]^0x808080,col[w][i],col[w][i]);
                for(int k=1000000000;k>1;k/=10){
                    if(k>num[w][i])fprintf(f,"&nbsp;");
                    else break;
                }
                double d=num[w][i]*100.0/total;
                fprintf(f,"%dpixels&nbsp;(%s%.3lf%)</div>\n",num[w][i],d<10.0?"&nbsp;":"",d);
            }
            fprintf(f,"        </p>\n");
        }
        fprintf(f,"    </body>\n</html>\n");
    }
};

inline void crowd(std::string pth,double limit=0.01,int need=Q_MAX){
    printf("colcard.crowd(%s,%.2lf,%d)\n",pth.c_str(),limit,need);
    if(OUTPUT&OUTPUT_PROGRESS)printf("Loading...\n");
    BMPrgb24*img=new BMPrgb24(pth);

    if(OUTPUT&OUTPUT_PROGRESS)printf("Initializing...\n");
    int*inverse=(int*)malloc(1<<29);//1<<1<<24<<2<<2
    memset(inverse,0,1<<29);
    
    if(OUTPUT&OUTPUT_PROGRESS)printf("Analyzing...\n");
    for(int i=0;i<img->size;i++){
        inverse[(1<<24|img->getpixel(i))<<2]++;
    }

    if(OUTPUT&OUTPUT_PROGRESS)printf("Sorting...\n");
    Q*q=new Q(img->size,limit,need);
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
    if(OUTPUT&OUTPUT_HTML)q->draw(img->read_path,img->origin_path);
    free(img);
    free(q);

    if(OUTPUT&OUTPUT_PROGRESS)printf("Ending...\n\n");
}

inline void naive(std::string pth,int needx=4,int needy=4){
    printf("colcard.naive(%s,%d,%d)\n",pth.c_str(),needx,needy);
    if(OUTPUT&OUTPUT_PROGRESS)printf("Loading...\n");
    const int need=needx*needy;
    if(0>=need||need>=Q_MAX)check_code(ERR_INPUT_SIZE_1);
    BMPrgb24*img=new BMPrgb24(pth,needx,needy);

    if(OUTPUT&OUTPUT_PROGRESS)printf("Analyzing...\n");
    Q*q=new Q(need,0,need);
    for(int i=0;i<img->size;i++){
        q->insert(1,1,img->getpixel(i));
    }

    if(OUTPUT&OUTPUT_LIST)q->show();
    if(OUTPUT&OUTPUT_PROGRESS)printf("Saving...\n");
    if(OUTPUT&OUTPUT_HTML)q->draw(img->read_path,img->origin_path);
    free(img);
    free(q);

    if(OUTPUT&OUTPUT_PROGRESS)printf("Ending...\n\n");
}

inline void line(std::string pth,int dis=96,int onlyline=0,int color=-1){
    printf("colcard.line(%s,%d,%d,%d)\n",pth.c_str(),dis,onlyline,color);
    if(OUTPUT&OUTPUT_PROGRESS)printf("Loading...\n");
    BMPrgb24*img=new BMPrgb24(pth);

    if(dis>=765){
        if(color>0)img->wgray(color);
        if(OUTPUT&OUTPUT_PROGRESS)printf("Saving...\n");
        if(OUTPUT&OUTPUT_PIC)img->save(pth+".765"+(onlyline?"_l":"")+".png");
        free(img);
        if(OUTPUT&OUTPUT_PROGRESS)printf("Ending...\n\n");
        return;
    }

    if(OUTPUT&OUTPUT_PROGRESS)printf("Initializing...\n");
    int*bfsx=(int*)malloc(img->size<<2);
    int*bfsy=(int*)malloc(img->size<<2);
    int*bfsz=(int*)malloc(img->size<<2);
    int t=0,cls=0;

    if(OUTPUT&OUTPUT_PROGRESS)printf("Analyzing...\n");
    const int delta_x[4]={1,-1,0,0},delta_y[4]={0,0,1,-1};
    for(int i=0;i<img->height;i++){
        for(int j=0;j<img->width;j++){
            if(img->getag(j,i))continue;
            img->setag(j,i,++cls);
            bfsx[t]=j;
            bfsy[t]=i;
            bfsz[t]=img->getpixel(j,i);
            t++;
            for(int k=t-1;k<t;k++){
                const int x=bfsx[k],y=bfsy[k],z=bfsz[k];
                for(int delta_z=0;delta_z<4;delta_z++){
                    const int xx=x+delta_x[delta_z],yy=y+delta_y[delta_z];
                    const int pp=img->getp(xx,yy);
                    if(pp<0)continue;
                    const int col=img->getpixel(pp);
                    if(distance(z,col)>dis)continue;
                    if(img->getag(pp))continue;
                    img->setag(xx,yy,cls);
                    bfsx[t]=xx;
                    bfsy[t]=yy;
                    bfsz[t]=col;
                    t++;
                }
            }
        }
    }

    if(OUTPUT&OUTPUT_PROGRESS)printf("Drawing...\n");
    for(int i=0;i<img->height;i++){
        for(int j=0;j<img->width;j++){
            int flg=0;
            const int tg=img->getag(j,i);
            for(int delta_z=0;delta_z<4;delta_z++){
                const int pp=img->getp(j+delta_x[delta_z],i+delta_y[delta_z]);
                if(pp<0)continue;
                const int tg2=img->getag(pp);
                if(tg^tg2)flg++;
            }
            if(flg){
                img->setpixel(j,i,0x000000);
            }else if(onlyline){
                img->setpixel(j,i,0xffffff);
            }
        }
    }
    if(color>0)img->wgray(color);

    if(OUTPUT&OUTPUT_PROGRESS)printf("Saving...\n");
    if(OUTPUT&OUTPUT_PIC)img->save(pth+"."+std::to_string(dis)+(onlyline?"_l":"")+".png");
    free(img);

    if(OUTPUT&OUTPUT_PROGRESS)printf("Ending...\n\n");
}

inline void linear(std::string pth,int rgb_black0,int rgb_white0,int rgb_black1,int rgb_white1,int con=0b111){
    printf("colcard.linear(%s,%x,%x,%x,%x,%d)\n",pth.c_str(),rgb_black0,rgb_white0,rgb_black1,rgb_white1,con);
    if(OUTPUT&OUTPUT_PROGRESS)printf("Loading...\n");
    BMPrgb24*img=new BMPrgb24(pth);

    if(OUTPUT&OUTPUT_PROGRESS)printf("Drawing...\n");
    img->linear(rgb_black0,rgb_white0,rgb_black1,rgb_white1,con);

    if(OUTPUT&OUTPUT_PROGRESS)printf("Saving...\n");
    if(OUTPUT&OUTPUT_PIC)img->save(pth+".linear.png");
    free(img);

    if(OUTPUT&OUTPUT_PROGRESS)printf("Ending...\n\n");
}