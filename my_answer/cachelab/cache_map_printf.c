#include <stdio.h>
#include <stdlib.h>
int main(int argc,char *argv[]){
    int i=0,j=0;
    int loc_A=0,loc_B=0;
    if(argc!=2) printf("usage :%s <M-size>\n",argv[0]);
    int M_size=atoi(argv[1]);
    if(M_size==32){
        for(i=0;i<32;i++){
            for(j=0;j<32;j++){
                loc_A=(i%8)*4+j/8;
                loc_B=(j%8)*4+i/8;
                int res=0;
                if(loc_A==loc_B) res=1;
                // printf("A:%02d  B:%2d res:%d ",loc_A,loc_B,res);
                printf("%d ",res);
            }
            printf("\n");
        }
    }
    else if(M_size==64){
        for(i=0;i<64;i++){
            for(j=0;j<64;j++){
                loc_A=(i%4)*8+j/8;
                loc_B=(j%4)*8+i/8;
                int res=0;
                if(loc_A==loc_B) res=1;
                // printf("A:%02d  B:%2d res:%d ",loc_A,loc_B,res);
                printf("%d ",res);
                //printf("%02d ",loc_A);
            }
        printf("\n");
        }
    }
    else printf("don't know\n");
    return 0;
}