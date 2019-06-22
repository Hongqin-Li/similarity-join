
#include <stdio.h>
#include <string.h>

#include "postgres.h"
#include "fmgr.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int d[1000][1000] = {0};

unsigned int _levenshtein_distance(const char *a, const char *b, const size_t len_a, const size_t len_b)
{
    unsigned int d[len_b+1][len_a+1];
    d[0][0] = 0;
    
    for (int i = 1; i <= len_b; i ++)
        d[i][0] = d[i-1][0] + 1;
    for (int j = 1; j <= len_a; j ++)
        d[0][j] = d[0][j-1] + 1;

    for (int i = 1; i <= len_b; i ++)
        for (int j = 1; j <= len_a; j ++)
            d[i][j] = MIN3(d[i-1][j] + 1, d[i][j-1] + 1, d[i-1][j-1] + (a[j-1] == b[i-1] ? 0 : 1));

    return d[len_b][len_a];
}

//2-Gram
float _jaccard_index(const char *a, const char *b, const size_t len_a, const size_t len_b)
{
    /* Remember to add \n at the end of a and b */
    printf("len_a: %d\nlen_b: %d\n", len_a, len_b);

    char dict[128][128] = {0};

    int intersect_cnt = 0;

    dict['$'][a[0]] ++;
    for (int i = 0; i < len_a - 1; i ++) 
        dict[a[i]][a[i+1]] ++;
    dict[a[len_a-1]]['$'] ++;

    if (dict['$'][b[0]] != 0) intersect_cnt ++;
    for (int i = 0; i < len_b - 1; i ++) 
        if (dict[b[i]][b[i+1]] != 0)
            intersect_cnt ++;
    if (dict[b[len_b-1]]['$'] != 0) intersect_cnt ++;

    printf("Intersect: %d\n", intersect_cnt);

    return (float)intersect_cnt / (len_a + len_b + 2 - intersect_cnt);
}


//Version 1 CAlling Conventions
PG_FUNCTION_INFO_V1(levenshtein_distance);

Datum levenshtein_distance(PG_FUNCTION_ARGS)
{
    text *s1_text = PG_GETARG_TEXT_PP(0);
    text *s2_text = PG_GETARG_TEXT_PP(1);

    const char *s1 = VARDATA(s1_text);
    const char *s2 = VARDATA(s2_text);

    int len_s1 = VARSIZE_ANY_EXHDR(s1_text);
    int len_s2 = VARSIZE_ANY_EXHDR(s2_text);
    
    PG_RETURN_INT32(_levenshtein_distance(s1, s2, len_s1, len_s2));
}


PG_FUNCTION_INFO_V1(jaccard_index);

Datum jaccard_index(PG_FUNCTION_ARGS)
{
    text *s1_text = PG_GETARG_TEXT_PP(0);
    text *s2_text = PG_GETARG_TEXT_PP(1);

    const char *s1 = VARDATA(s1_text);
    const char *s2 = VARDATA(s2_text);

    int len_s1 = VARSIZE_ANY_EXHDR(s1_text);
    int len_s2 = VARSIZE_ANY_EXHDR(s2_text);

    PG_RETURN_FLOAT8(_jaccard_index(s1, s2, len_s1, len_s2));

}


/*
int main() 
{
    char a[100] = "abc";
    char b[100] = "abc";

    printf("Jaccard distance: %f\n", _jaccard_index(a, b, strlen(a), strlen(b)));
    printf("Levenshtein distance: %d\n", _levenshtein_distance(a, b, strlen(a), strlen(b)));

}

*/



