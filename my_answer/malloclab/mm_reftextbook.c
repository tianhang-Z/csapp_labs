/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "thz",
    /* First member's email address */
    "caiji@edu.cn",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* 基础宏*/
#define WSIZE 4 // 字长4字节，块头尾信息也为4字节
#define DSIZE 8
#define CHUNKSIZE (1 << 12) // 堆初始化大小 4KB 正好是页大小

#define MAX(x, y) ((x) > (y)) ? (x) : (y)
/* 把块大小和信息位结合*/
#define PACK(size, alloc) ((size) | (alloc))
/* 在地址p处读写 */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* 从头部或脚部 获取块大小和信息位的值 */
#define GET_SIZE(p) (GET(p) & ~0x07)
#define GET_ALLOC(p) (GET(p) & 0x1)

/*给定块指针bp，其指向有效载荷，计算头部和尾部指针*/
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* 给定块指针bp, 计算先前块和后面块的地址 */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

char *heap_listp = NULL;
void *extend_heap(size_t size);
void *coalesce(void *bp);
void *find_first_fit(size_t asize);
void place(void *bp, size_t asize);

/*-------------------------------help function-----------------------*/
/* 扩展堆 以字节为单位*/
void *extend_heap(size_t size)
{
    char *bp;

    size = ALIGN(size + 8);
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL; // 申请失败

    PUT(HDRP(bp), PACK(size, 0)); // 设置头部和脚步
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // 设置结尾块

    // 扩展堆之后，结尾块后面紧跟的就是有效载荷，结尾块变成该载荷的头部
    return coalesce(bp);
}

// 合并空闲块 序言块和结尾块 有效避免了边界处理
// 参数bp指向有效载荷
void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc)
    {
        return bp;
    }
    else if (prev_alloc && !next_alloc)
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc)
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

// find fit free block
void *find_first_fit(size_t asize)
{
    void *bp;
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp))))
            return bp;
    }
    return NULL;
}

// replace free block
void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp)); // 空闲块大小

    // 剩余块大于等于16字节 则需要分割空闲块 否则直接作为内部碎片
    if ((csize - asize) >= 2 * DSIZE)
    {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
    }
    else
    {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

/*-------------------------------------end help function------------------------ */
/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    // 申请四个字 用于存储起始块、结尾块和序言块
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);
    PUT(heap_listp + WSIZE, PACK(8, 1));
    PUT(heap_listp + 2 * WSIZE, PACK(8, 1));
    PUT(heap_listp + 3 * WSIZE, PACK(0, 1));
    heap_listp += (2 * WSIZE);
    // 申请一个初始的空闲块
    if (extend_heap(CHUNKSIZE) == NULL)
        return -1; // 申请失败
    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;       // adjust block size
    size_t extend_size; // extend heap size
    char *bp;

    if (size == 0)
        return NULL;

    // 块最小为16字节   有效载荷最小为8字节  要求按八字节对齐
    // 有效载荷小于8 则按16处理；大于8，则加8后向上舍入到8的倍数
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + DSIZE - 1) / DSIZE) + DSIZE;

    // 寻找合适的空闲块
    if ((bp = find_first_fit(asize)) != NULL)
    {
        place(bp, asize);
        return bp;
    }

    // 没找到合适块 则需要扩展堆 按至少4kB大小来扩展
    extend_size = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extend_size)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    // copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    copySize = GET_SIZE(HDRP(ptr));

    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}
