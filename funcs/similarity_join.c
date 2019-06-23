#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MAX_LEN 128

void to_lower(const char *s, const size_t len, char *r)
{
    for (int i = 0; i < len; i ++)
        r[i] = tolower(s[i]); 
}

int _levenshtein_distance(const char *a, const char *b, const int len_a, const int len_b)
{
    static int d[MAX_LEN][MAX_LEN];
    d[0][0] = 0;

    /* Initialize the first row and column */
    for (int i = 1; i <= len_b; i ++)
        d[i][0] = i;
    for (int j = 1; j <= len_a; j ++)
        d[0][j] = j;

    for (int i = 1; i <= len_b; i ++) 
        for (int j = 1; j <= len_a; j ++)
            d[i][j] = MIN3(d[i-1][j] + 1, d[i][j-1] + 1, d[i-1][j-1] + (a[j-1] == b[i-1] ? 0 : 1));

    return d[len_b][len_a];
}

int _levenshtein_distance_less_than(const char *a, const char *b, const int len_a, const int len_b, int k)
{
    static int d[MAX_LEN][MAX_LEN];
    d[0][0] = 0;

    /* Initialize the first row and column */
    for (int i = 1; i <= len_b; i ++)
        d[i][0] = i;
    for (int j = 1; j <= len_a; j ++)
        d[0][j] = j;

    for (int i = 1; i <= len_b; i ++) 
    {
        int all_ge_k = 1;
        for (int j = 1; j <= len_a; j ++) 
        {
            d[i][j] = MIN3(d[i-1][j] + 1, d[i][j-1] + 1, d[i-1][j-1] + (a[j-1] == b[i-1] ? 0 : 1));
            if (d[i][j] < k) all_ge_k = 0;
        }
        if (all_ge_k)
            return 0;
    }
    return d[len_b][len_a] < k;

}

//2-Gram
float _jaccard_index(const char *a, const char *b, const int len_a, const int len_b)
{
    //printf("len_a: %d\nlen_b: %d\n", len_a, len_b);

    char dict[256][256] = {0};

    int intersect_cnt = 0;
    
    dict['$'][a[0]] ++;
    for (int i = 1; i < len_a; i ++) 
        dict[a[i-1]][a[i]] ++;
    dict[a[len_a-1]]['$'] ++;

    if (dict['$'][b[0]] != 0) intersect_cnt ++;
    for (int i = 1; i < len_b; i ++) 
        if (dict[b[i-1]][b[i]] != 0)
            intersect_cnt ++;
    if (dict[b[len_b-1]]['$'] != 0) intersect_cnt ++;

    //printf("Intersect: %d\n", intersect_cnt);

    return (float)intersect_cnt / (len_a + len_b + 2 - intersect_cnt);
}

#ifdef DEBUG


int main() 
{
    char a[100];
    char b[100];
    scanf("%s", a);
    scanf("%s", b);
    printf("Jaccard distance: %f\n", _jaccard_index(a, b, strlen(a), strlen(b)));
    printf("Levenshtein distance: %d\n", _levenshtein_distance(a, b, strlen(a), strlen(b)));

}

#else

#include "postgres.h"
#include "fmgr.h"
#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

//Version 1 Calling Conventions
PG_FUNCTION_INFO_V1(levenshtein_distance);

Datum levenshtein_distance(PG_FUNCTION_ARGS)
{
    text *s1_text = PG_GETARG_TEXT_PP(0);
    text *s2_text = PG_GETARG_TEXT_PP(1);

    const char *s1 = VARDATA_ANY(s1_text);
    const char *s2 = VARDATA_ANY(s2_text);

    int len_s1 = VARSIZE_ANY_EXHDR(s1_text);
    int len_s2 = VARSIZE_ANY_EXHDR(s2_text);

    char s1_lower[len_s1];
    char s2_lower[len_s2];
    to_lower(s1, len_s1, s1_lower);
    to_lower(s2, len_s2, s2_lower);

    PG_RETURN_INT32(_levenshtein_distance(s1_lower, s2_lower, len_s1, len_s2));
}

PG_FUNCTION_INFO_V1(levenshtein_distance_less_than);

Datum levenshtein_distance_less_than(PG_FUNCTION_ARGS)
{
    text *s1_text = PG_GETARG_TEXT_PP(0);
    text *s2_text = PG_GETARG_TEXT_PP(1);
    int k = PG_GETARG_INT32(2);

    const char *s1 = VARDATA_ANY(s1_text);
    const char *s2 = VARDATA_ANY(s2_text);

    int len_s1 = VARSIZE_ANY_EXHDR(s1_text);
    int len_s2 = VARSIZE_ANY_EXHDR(s2_text);

    char s1_lower[len_s1];
    char s2_lower[len_s2];
    to_lower(s1, len_s1, s1_lower);
    to_lower(s2, len_s2, s2_lower);

    PG_RETURN_BOOL(_levenshtein_distance_less_than(s1_lower, s2_lower, len_s1, len_s2, k));
}




PG_FUNCTION_INFO_V1(jaccard_index);

Datum jaccard_index(PG_FUNCTION_ARGS)
{
    text *s1_text = PG_GETARG_TEXT_PP(0);
    text *s2_text = PG_GETARG_TEXT_PP(1);

    const char *s1 = VARDATA_ANY(s1_text);
    const char *s2 = VARDATA_ANY(s2_text);

    int len_s1 = VARSIZE_ANY_EXHDR(s1_text);
    int len_s2 = VARSIZE_ANY_EXHDR(s2_text);

    char s1_lower[len_s1];
    char s2_lower[len_s2];
    to_lower(s1, len_s1, s1_lower);
    to_lower(s2, len_s2, s2_lower);

    PG_RETURN_FLOAT8(_jaccard_index(s1_lower, s2_lower, len_s1, len_s2));
}

#endif




