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

//
// Tranpose 32x32 matrix
//
void trans_32x32(int M, int N, int A[N][M], int B[M][N]) {
    unsigned char bw = 8, bh = 8;
    unsigned int i, j, ii, jj;
    unsigned int temp_ii, temp_jj;
    int temp;

    for (i = 0; i < N; i += bh) {
        for (j = 0; j < M; j += bw) {
        
            // move block

            for (ii = i; ii < i + bh && ii < N; ++ii) {
                for (jj = j; jj < j + bw && jj < M; ++jj) {
                    if (ii != jj) B[jj][ii] = A[ii][jj];
                    else {
                        temp_ii = ii;
                        temp_jj = jj;
                        temp = A[ii][jj];
                    }
                }
                if (i == j) B[temp_ii][temp_jj] = temp;
            }
        }
    }
}

// 
// Tranponse 32x64 matrix
//
void trans_32x64(int M, int N, int A[N][M], int B[M][N]) {
    unsigned char bw = 4, bh = 8;
    unsigned int i, j, ii, jj;

    for (i = 0; i < N; i += bh) {
        for (j = 0; j < M; j += bw) {

            // move block

            for (ii = i; ii < i + bh && ii < N; ++ii) {
                for (jj = j; jj < j + bw && jj < M; ++jj) {
                    B[jj][ii] = A[ii][jj];
                }
            }
        }
    }
}

//
// Tranpose 64x64 matrix
//
void trans_64x64(int M, int N, int A[N][M], int B[M][N]) {
    unsigned char bw = 4, bh = 8;
    unsigned int i, j, ii, jj;

    for (i = 0; i < N; i += bh) {
        for (j = 0; j < M; j += bw) {
        
            // move block

            for (ii = i; ii < i + bh && ii < N; ++ii) {
                for (jj = j; jj < j + bw && jj < M; ++jj) {
                    B[jj][ii] = A[ii][jj];
                }
            }
        }
    }
}

/*
 * transpose_submit - This is the solution transpose function that
 *     you will be graded on the assignment. Do not change the
 *     description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    if (N == 32) trans_32x32(M, N, A, B);
    else if (M == 32 && N == 64) trans_32x64(M, N, A, B);
    else if (M == 64 && N == 64) trans_64x64(M, N, A, B);
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

