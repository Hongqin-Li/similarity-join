#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "postgres.h"
#include "fmgr.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

#define MAX_LEN 128
#define MAXH 100
#define MAXN 1000

typedef struct Node
{
    char a;
    char b;
    int cnt;
    int nxt;
} Node;

static Node node[MAXN];
static int ni;
static int hash_table[MAXH];

//2-gram
static int hash(char a, char b)
{
    return (a*2 + b) % MAXH;
}

static void hash_init()
{
    ni = 0;
    for (int i = 0; i < MAXH; i ++)
        hash_table[i] = 0;
}

static void hash_insert(char a, char b)
{
    int key = hash(a, b);

    int *pi = &hash_table[key];
    while (*pi)
    {
        if (node[*pi].a == a && node[*pi].b == b)
        {
            node[*pi].cnt ++;
            return;
        }
        pi = &node[*pi].nxt;
    }
    node[++ni].a = a;
    node[ni].b = b;
    node[ni].nxt = 0;
    node[ni].cnt = 1;
    *pi = ni;
}

static int hash_delete(char a, char b)
{
    int key = hash(a, b);
    int *pi = &hash_table[key];
    while (*pi)
    {
        Node now = node[*pi];
        if (now.a == a && now.b == b)
        {
            if (now.cnt)
            {
                node[*pi].cnt --;
                return 1;
            }
            else return 0;
        }
        pi = &node[*pi].nxt;
    }
    return 0;
}

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

/* Optimized Levenshtein distance by early stop */
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
        int all_ge_k = 1;//All greater or equal to k
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


/* Hash table: much faster than naive look-up table */
float _jaccard_index(const char *a, const char *b, const int len_a, const int len_b)
{
    int intersect_cnt = 0;

    hash_init();
    
    hash_insert('$', a[0]);
    for (int i = 1; i < len_a; i ++) 
        hash_insert(a[i-1], a[i]);
    hash_insert(a[len_a-1], '$');

    if (hash_delete('$', b[0])) intersect_cnt ++;
    for (int i = 1; i < len_b; i ++) 
        if (hash_delete(b[i-1], b[i]))
            intersect_cnt ++;
    if (hash_delete(b[len_b-1], '$')) intersect_cnt ++;

    return (float)intersect_cnt / (len_a + len_b + 2 - intersect_cnt);
}



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





