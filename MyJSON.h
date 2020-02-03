//in the name of god

typedef struct {
    char text[100000];
    char valuestring[100000];

} cJSON;

void Delete_MyJSON(cJSON* a);

//client functions

cJSON* cJSON_Parse(char* t);

int Find_State(char* f);
cJSON* cJSON_GetObjectItem(cJSON* ptr, char* t);
cJSON* cJSON_GetArrayItem(cJSON* arr, int i);

int cJSON_GetArraySize(cJSON* arr);

//server functions
cJSON* cJSON_CreateObject();
cJSON* cJSON_CreateArray();
cJSON* cJSON_CreateString(char* str);

void EndObject(cJSON* a);
void EndArray(cJSON* b);
void EndObjectwithArray(cJSON* a);

void cJSON_AddItemToObject(cJSON* a, char* item, cJSON* b);
void cJSON_AddItemToArray(cJSON* b, cJSON* a);

char* cJSON_Print(cJSON* a);
