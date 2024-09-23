/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if(M==32&&N==32){
        int i=0,j=0;
        int block_i=0;
        for (i = 0; i < 32; i+=8) {
            for (j = 0; j < 32; j+=8) {
                for(block_i=i;block_i<i+8;block_i++){
                    int A0=A[block_i][j];
                    int A1=A[block_i][j+1];
                    int A2=A[block_i][j+2];
                    int A3=A[block_i][j+3];
                    int A4=A[block_i][j+4];
                    int A5=A[block_i][j+5];
                    int A6=A[block_i][j+6];
                    int A7=A[block_i][j+7];
                    B[j][block_i]=A0;
                    B[j+1][block_i]=A1;
                    B[j+2][block_i]=A2;
                    B[j+3][block_i]=A3;
                    B[j+4][block_i]=A4;
                    B[j+5][block_i]=A5;
                    B[j+6][block_i]=A6;
                    B[j+7][block_i]=A7; 
                }
            }
        }
    }
    else if(M==64&&N==64){
        int i=0,j=0;
        int a_0, a_1, a_2, a_3, a_4, a_5, a_6, a_7;
        for (i = 0; i < 64; i+=8) {
            for (j = 0; j < 64; j+=8) {
                //步骤1
                for (int k = i; k < i + 4; k++){
                // 得到A的第1,2块
                a_0 = A[k][j + 0];
                a_1 = A[k][j + 1];
                a_2 = A[k][j + 2];
                a_3 = A[k][j + 3];
                a_4 = A[k][j + 4];
                a_5 = A[k][j + 5];
                a_6 = A[k][j + 6];
                a_7 = A[k][j + 7];
                // 复制给B的第1,2块
                B[j + 0][k] = a_0;
                B[j + 1][k] = a_1;
                B[j + 2][k] = a_2;
                B[j + 3][k] = a_3;
                B[j + 0][k + 4] = a_4;
                B[j + 1][k + 4] = a_5;
                B[j + 2][k + 4] = a_6;
                B[j + 3][k + 4] = a_7;
                }
                //步骤2，3 逐行处理B右上角，按列处理A的左下角
                //此处k表示A的列序号 即B的行序号
                for (int k = j; k < j + 4; k++){
                // 得到B的右上角块的每一行
                a_0 = B[k][i + 4];
                a_1 = B[k][i + 5];
                a_2 = B[k][i + 6];
                a_3 = B[k][i + 7];
                // 得到A的左下角的每一列
                a_4 = A[i + 4][k];
                a_5 = A[i + 5][k];
                a_6 = A[i + 6][k];
                a_7 = A[i + 7][k];
                // A的左下角列复制给B的右上角每一行
                B[k][i + 4] = a_4;
                B[k][i + 5] = a_5;
                B[k][i + 6] = a_6;
                B[k][i + 7] = a_7;
                // B的左上角每一行赋值给B的左下角每一行
                B[k + 4][i + 0] = a_0;
                B[k + 4][i + 1] = a_1;
                B[k + 4][i + 2] = a_2;
                B[k + 4][i + 3] = a_3;
                }
                //步骤4 最后一小块的转置
                for (int k = i + 4; k < i + 8; k++){
                    // 处理第4块
                    a_4 = A[k][j + 4];
                    a_5 = A[k][j + 5];
                    a_6 = A[k][j + 6];
                    a_7 = A[k][j + 7];
                    B[j + 4][k] = a_4;
                    B[j + 5][k] = a_5;
                    B[j + 6][k] = a_6;
                    B[j + 7][k] = a_7;
                }
            }
        }
    }
    else {
        for (int i = 0; i < N; i += 16) {
            for (int j = 0; j < M; j += 16) {
                for (int ii = i; ii < i + 16 && ii < N; ii++) {
                    for (int jj = j; jj < j + 16 && jj < M; jj++) {
                        int a0 = A[ii][jj];
                        B[jj][ii] = a0;
                    }
                }
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register any additional transpose functions */
   // registerTransFunction(trans, trans_desc); 
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

