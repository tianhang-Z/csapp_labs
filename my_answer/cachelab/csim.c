#define _XOPEN_SOURCE
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "cachelab.h"

#define MAX_SIZE 1024
int char2int(char c);
int main(int argc,char* argv[])
{
    int s=0,E=0,b=0;
    int debug=0;
    int S=0;
    //int B=0;
    char *trace_file;
    char *usage="Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile> \n\
                -h: Optional help flag that prints usage info \n\
                -v: Optional verbose flag that displays trace info \n\
                -s <s>: Number of set index bits (S = 2s is the number of sets) \n\
                -E <E>: Associativity (number of lines per set) \n\
                -b <b>: Number of block bits (B = 2b is the block size) \n\
                -t <tracefile>: Name of the valgrind trace to replay\n" ;
    //解析命令
    //加冒号的是必选 getopt自带全局变量optarg
    int opt=0;
    while((opt=getopt(argc,argv,"hvs:E:b:t:"))!=-1){
        switch (opt)
        {
        case 'h':
            printf("%s",usage);
            break;
        case 'v':
            debug=1;
            break;
        case 's':
            s=atoi(optarg);
            S=1<<s;
            break;
        case 'E':
            E=atoi(optarg);
            break;
        case 'b':
            b=atoi(optarg);
            //B=1<<b;
            break;
        case 't':
            trace_file=optarg;
            break;
        default:
            break;
        }
    }

    //申请cache空间  cache是一个S*E个缓存行（每个缓存行包含一个块及有效位 标记位 )
    typedef struct  cache_line_{
        int valid;   //有效位
        __uint64_t tag;   //标记位
        int last_access_time;  //为了LRU设置的访问时间记录
    }cache_line;

    cache_line *cache=(cache_line*)malloc(sizeof(cache_line)*S*E);
    for(int i=0;i<S*E;i++){
        cache[i].valid=0;
        cache[i].tag=0;
        cache[i].last_access_time=0;
    }


    FILE *fp=fopen(trace_file,"r");
    char line_of_trace[MAX_SIZE];
    int time_count=0;  //记录时间 每access一次内存 时间+1  时间越小 说明越久远
    //逐行处理trace

    int hit=0;
    int miss=0;
    int eviction=0;
    while(fgets(line_of_trace,MAX_SIZE,fp)){
        //分割每一行
        __uint64_t address=0;  
        //int size=0;    
        int access_count=0;

        int str_len=strlen(line_of_trace);
        char single_char;
        for(int i=0;i<str_len-1;i++){
            single_char=line_of_trace[i];
            if(single_char=='I')
                break;
            else if(single_char=='L'||single_char=='S')
                access_count=1;
            else if(single_char=='M')
                access_count=2;
            else if(single_char==' ')
                continue;
            else if(single_char==','){
                //size=line_of_trace[i+1]-'0';
                break;
            }
            else {
                address=address*16+char2int(single_char);
            }
        }
        if(debug&&access_count!=0) printf("%s",line_of_trace);

        if(access_count==0) continue;
        //printf("access time:%d,adr:%d,size:%d\n",access_count,address,size);
    
        //模拟内存访问 对于L，S只访问一次
        //对于M,则访问两次，且不论第一次结果如何，第二次必然hit
        time_count++;   //每次访问 时间递增
        
        //计算各个位；
        int m=64;
        __uint64_t tag=address>>(s+b);   //标记位
        __uint64_t S_index=(address<<(m-s-b))>>(m-s);  //组索引
        //__uint64_t block_offset=(address<<(m-b))>>(m-b);   //块偏移
        // printf("tag: %lx ,",tag);
        // printf("S_index:%lx ,",S_index);
        // printf("block_offset:%lx\n",block_offset);

        int hit_flag=0;
        int get_free_cache=0;
        if(access_count>=1){
            int cache_idx_st=S_index*E;  //cache的S_index组的起点偏移
            //查询该组的缓存行
            //查询是否hit
            for(int i=0;i<E;i++){   
                if(cache[cache_idx_st+i].valid==1&& \
                    cache[cache_idx_st+i].tag==tag){
                    hit++;
                    hit_flag=1;
                    cache[cache_idx_st+i].last_access_time=time_count; //记得更新时间
                    break;
                }
            }
            if(hit_flag&&debug)  printf(" hit");

            //未hit 查询是否有空cache_line
            if(!hit_flag){
                miss++;
                if(debug) printf(" miss");
                for(int i=0;i<E;i++){   
                if(cache[cache_idx_st+i].valid==0){
                    cache[cache_idx_st+i].valid=1;
                    cache[cache_idx_st+i].tag=tag;
                    cache[cache_idx_st+i].last_access_time=time_count;
                    get_free_cache=1;
                    break;
                    }
                }
            }
           
            //cache满了且miss 
            if(!get_free_cache&&!hit_flag){
                unsigned int evic_idx=0,min_time_count=0xffffffff;
                //找出last_access_time最小了 即上次访问最久远的 将其替换
                for(int i=0;i<E;i++){
                    if(cache[cache_idx_st+i].last_access_time<min_time_count){
                        min_time_count=cache[cache_idx_st+i].last_access_time;
                        evic_idx=i;
                    }
                }
                cache[cache_idx_st+evic_idx].tag=tag;
                cache[cache_idx_st+evic_idx].last_access_time=time_count;
                cache[cache_idx_st+evic_idx].valid=1;
                eviction++;
                if(debug) printf(" eviction"); 
            }

            if(access_count==2) {
                if(debug) printf(" hit");
                hit++;
                time_count++;
            }
        }
        if(debug) printf("\n");
    }
    //printf("\nhit:%d,miss:%d,eviction:%d\n",hit,miss,eviction);
    printSummary(hit,miss,eviction);
    return 0;
}

int char2int(char c){
    if(c<='9'&&c>='0')
        return c-'0';
    else if(c<='f'&&c>='a')
        return c-'a'+10;
    return 0;
}
