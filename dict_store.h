#ifndef _DICT_STORE_H
#define _DICT_STORE_H

typedef enum _KvType
{
    KV_NULL,
    KV_STRING,
    KV_OBJECT,
} KvType;

typedef struct _Kv
{
    char *key;
    union 
    {
        char *valueStr;
        struct _Kv *valueObj;
    };
    KvType type;

    struct _Kv *next;
} Kv;

typedef struct _DictStore 
{
    Kv *head;
} DictStore;

typedef struct _KvParser 
{
    char *buffer;
    int   curOffset;
    int   bufLen;

    Kv *cur;
    KvType curType;
} KvParser;

#define isWhiteSpace(ch) \
    (ch == ' ' || ch == '\r' || \
     ch == '\n' || ch == '\t' || \
     ch == '\v' || ch == '\f')

int dictParseRaw(char *rawDictStr, int len, DictStore *store);
int dictStorePrint(DictStore *store);

int dictParse(KvParser *p, DictStore *store);
int dictParserInit(KvParser *p, int len, char *rawDictStr);
int dictWalkWhiteSpace(KvParser *p);
int dictWalkOneStep(KvParser *p);
char dictPeek(KvParser *p); // int dictPeek2(KvParser *p, char *ch);
Kv *kvCreate(void); // int kvCreate(Kv **kv);
Kv *dictParseObject(KvParser *p); //int dictParseObject(KvParser *p, Kv **kv);
int *dictParseValue(KvParser *p); 
char  *dictParseString(KvParser *p); // int dictParseString(KvParser *p, char **str);
int dictWalkToTerminator(KvParser *p, char ch);
int dictParserSetExpectType(KvParser *p);


#endif