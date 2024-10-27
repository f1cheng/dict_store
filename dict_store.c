#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "dict_store.h"

int kvCreate2(Kv **kv)
{
    *kv = malloc(sizeof(Kv));
    (*kv)->key = NULL;
    (*kv)->next = NULL;
    (*kv)->type = KV_NULL;
    return 0;
}

Kv *kvCreate(void)
{
    Kv *kv;
    kv = malloc(sizeof(Kv));
    kv->key = NULL;
    kv->next = NULL;
    kv->type = KV_NULL;
    return kv;
}

int dictParserInit(KvParser *p, int len, char *rawDictStr)
{
    p->buffer = rawDictStr;
    p->curOffset = 0;
    p->bufLen = len;
    p->curType = KV_NULL;
    p->cur = NULL;
    return 0;
}

int dictStorePrint(DictStore *store)
{
    Kv *kv = store->head;
    while (kv)
    {
        if (kv->type == KV_STRING)
        {
            printf("key:value %s:%s\n", kv->key, kv->valueStr);
        }
        kv = kv->next;
    }
    return 0;
}

char dictPeek(KvParser *p)
{
    return p->buffer[p->curOffset];
}

int dictParse(KvParser *p, DictStore *store)
{
    dictWalkWhiteSpace(p);
    char peek = dictPeek(p);
    Kv *root = kvCreate();
    p->cur = root;

    if (peek == '{')
    {
        root->type = KV_OBJECT;
        root->valueObj = dictParseObject(p);
    }
    store->head = root;
    return 0;
}

int dictParseRaw(char *rawDictStr, int len, DictStore *store)
{
    KvParser parser;
    dictParserInit(&parser, len, rawDictStr);
    dictParse(&parser, store);
    return 0;
}

int *dictParseValue(KvParser *p)
{
    int ret = 1;
    Kv *kv = p->cur;
    
    switch (p->curType) 
    {
        case KV_STRING:
            kv->type = KV_STRING;
            kv->valueStr = dictParseString(p);
            break;
        case KV_OBJECT:
            kv->type = KV_OBJECT;
            kv->valueObj = dictParseObject(p);
            break;
        default:
            break;
    }
    return 0;
}

char  *dictParseString(KvParser *p)
{
    int start, end, len;
    if (dictPeek(p) == '"')
    {
        dictWalkOneStep(p);
        start = p->curOffset;
    }
    else
    {
        char cc = dictPeek(p);
        printf("Unexpect %c\n", cc);
        return "";
    }

    while (p->curOffset < p->bufLen)
    {
        if (p->buffer[p->curOffset] == '"' )
        {
            end = p->curOffset;
            break;
        }
        else
        {
            end = p->curOffset;
            p->curOffset++;
        }
    }
    
    len = sizeof(char) * (end - start);
    if (len == 0)
    {
        return "";
    }
    
    char *str = malloc(len);
    // memcpy(str, p->buffer[start], len);
    for (int i = 0; (start + i) < end; i++)
    {
        *(str + i) = p->buffer[start+i];
    }
    return str;
}

int dictWalkOneStep(KvParser *p)
{
    if ((p->curOffset + 1) < p->bufLen)
    {
        p->curOffset++;
    }
    return 0;
}

int dictWalkWhiteSpace(KvParser *p)
{
    if ((p->curOffset + 1) >= p->bufLen)
    {
        return 0;
    }
    // is white space
    while (isWhiteSpace(dictPeek(p)))
    {
        p->curOffset++;
    }
    return 0;
}

int dictWalkToTerminator(KvParser *p, char ch)
{
    while ((dictPeek(p) != ch) && (p->curOffset < p->bufLen - 1))
    {
        p->curOffset++;
    }
    return 0;
}

int dictParserSetExpectType(KvParser *p)
{
    char peek = dictPeek(p);
    switch (peek)
    {
        case '{':
            p->curType = KV_OBJECT;
            break;
        case '"':
            p->curType = KV_STRING;
            break;
        default:
            break; 
    }
    return 0;
}

Kv *dictParseObject(KvParser *p)
{
    Kv *kv, *firstKv;

    dictWalkOneStep(p);
    dictWalkWhiteSpace(p);
    if (dictPeek(p) == '}')
    {
        //p->err = EOF;
        return NULL;
    }

    firstKv = kvCreate();
    p->cur->next = firstKv;
    p->cur = firstKv;

    while (1)
    {
        kv = p->cur;
        kv->key = dictParseString(p);

        dictWalkToTerminator(p, ':');

        dictWalkOneStep(p);
        dictWalkWhiteSpace(p);

        dictParserSetExpectType(p);
        dictParseValue(p);

        dictWalkOneStep(p);
        dictWalkWhiteSpace(p);
        char ch = dictPeek(p);
        if (ch == '}')
        {
            break; 
        }
        else if (ch == ',')
        {
            dictWalkOneStep(p);
            dictWalkWhiteSpace(p);
        }
        else
        {
            break;
        }
        //next kv parser
        kv->next = kvCreate();
        p->cur = kv->next; 
    }
    return firstKv; 
}
