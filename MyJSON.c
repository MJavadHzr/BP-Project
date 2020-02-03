//in the name of god

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "MyJSON.h"


void Delete_MyJSON(cJSON* a)
{
    free(a);
}

//client JSON functions
cJSON* cJSON_Parse(char* t)
{
    cJSON* ptr = (cJSON*) malloc(sizeof(cJSON));
    strcpy(ptr->text, t);
    return ptr;
}

int Find_State(char* find)
{
    for( ; *find != ':' ; ) find++;
    if(*(find+1) == '\"') return 1;
    return 2;
}

cJSON* cJSON_GetObjectItem(cJSON* ptr, char* t)
{
    char search[50];
    sprintf(search, "\"%s\"", t);

    char* c;//for content
    int flag = 1;//for content


    //save item text in cJSON text
    char* find = strstr(ptr->text, search);

    cJSON* item = (cJSON*) malloc(sizeof(cJSON));
    int state = Find_State(find);
    int i=1, j=0;
    for( ; ((state == 1 && *find != '\n') || (state == 2 && (*find != '\n' || *(find-1) != ']'))) ; j++)
    {
        item->text[j] = *find;
        find++;
        i += (*find == '\"' ? 1 : 0);
        //find first of content
        if(i == 3 && flag)
        {
            c = find;
            flag = 0;
        }
    }
    item->text[j] = '\0';

    //save item content in value-string
    int k=0;
    c++;
    for( ; *c != '\"' ; k++)
    {
        item->valuestring[k] = *c;
        c++;
    }
    item->valuestring[k] = '\0';
    return item;
}


int cJSON_GetArraySize(cJSON* arr)
{
    //check what array have(strings or objects)
    char *find = strstr(arr->text, "[");
    int state;
    if(*(find+1) == '\"') state = 1;
    else if(*(find+1) == '{') state = 2;
    else state = 3;
    //count array item
    int count = 0;
    for(int k=0 ; arr->text[k] != 93 ; k++)
    {
        if((state == 1 && arr->text[k] == ',') || (state == 2 && arr->text[k] == ',' && arr->text[k-1] == 125)){
            count++;
        }
    }
    if(state != 3)
        return count+1;
    return 0;
}

cJSON* cJSON_GetArrayItem(cJSON* arr, int i)
{
    char* find = strstr(arr->text, "[");

    int state = 0;
    if(*(find+1) == '\"') state = 1;
    else if(*(find+1) == 123) state = 2;

    find ++;
    char* move = find;
    int count = -1;
    for(int j=0 ; *move != 93 ; j++)
    {
        move++;
        if((state == 1 && *move == ',') || (state == 2 && *move == ',' && *(move-1) == 125)){
            count++;
            if(count == i) break;
            else{
                find = move +1;
            }
        }
    }
    cJSON* arritem = (cJSON*) malloc(sizeof(cJSON));
    memset(arritem->text, 0, sizeof(arritem->text));
    strncat(arritem->text, find, move-find);

    //for arr item
    int k=0, flag=0;
    for(int j=0 ; flag != 2 ; j++)
    {
        if(arritem->text[j] == '\"'){
             flag++;
             continue;
        }
        if(flag)
        {
            arritem->valuestring[k] = arritem->text[j];
            k++;
        }

    }
    arritem->valuestring[k] = '\0';
    return arritem;
}
//end client



//Server JSON functions
cJSON* cJSON_CreateObject()
{
    cJSON* ptr = (cJSON*) malloc(sizeof(cJSON));
    ptr->text[0] = 123;
    return ptr;
}

cJSON* cJSON_CreateArray()
{
    cJSON* ptr = (cJSON*) malloc(sizeof(cJSON));
    ptr->text[0] = '[';
    return ptr;
}

cJSON* cJSON_CreateString(char* str)
{
    cJSON* ptr = (cJSON*) malloc(sizeof(cJSON));
    ptr->text[0] = '"';
    strcat(ptr->text, str);
    strcat(ptr->text, "\"");
    return ptr;
}

void EndObject(cJSON* a)
{
    strcat(a->text, "}");
    char* ptr = strstr(a->text, "}");
    *(ptr-1) = '\n';
}

void EndArray(cJSON* b)
{
    strcat(b->text, "]");
    char* ptr = strstr(b->text, "]");
    ptr--;
    *ptr = *(ptr+1);
    *(ptr+1) = *(ptr+2);
}

void EndObjectwithArray(cJSON* a)
{
    char* ptr = strstr(a->text, "]");
    *(ptr+1) = '\n';
    *(ptr+2) = 125;
    *(ptr+3) = '\0';
}

void cJSON_AddItemToObject(cJSON* a, char* item, cJSON* b)
{
    strcat(a->text, "\n\t\"");
    strcat(a->text, item);
    strcat(a->text, "\":");
    strcat(a->text, b->text);
    strcat(a->text, ",");
}

void cJSON_AddItemToArray(cJSON* b, cJSON* a)
{
    strcat(b->text, a->text);
    strcat(b->text, ",");
}

char* cJSON_Print(cJSON* a)
{
    return a->text;
}
