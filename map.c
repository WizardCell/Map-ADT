#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define INITIAL_SIZE 10
#define EXPAND_FACTOR 2

/* Helper Functions */
static char* copyString(const char* str);
static MapResult expand(Map map);

/** Type for defining an element of a map */
typedef struct Element_t
{
    char* key;
    char* value;
} *Element;

/** Type for defining the map */
struct Map_t
{
    Element* elements;
    int size;
    int maxSize;
    int iterator;
};

int main()
{
    Map map = mapCreate();

    assert(mapPut(map, "key1", "value1") == MAP_SUCCESS);
    assert(mapPut(map, "key2", "value2") == MAP_SUCCESS);

    int i = 0;
    MAP_FOREACH(iter, map)
    {
        if (i != 2)
        {
            assert(iter != NULL);
        }
        else
        {
            assert(iter == NULL);
        }
        i++;
    }

    mapDestroy(map);

    return 0;
}

Map mapCreate()
{
    Map map = malloc(sizeof(*map));
    if (map == NULL)
    {
        return NULL;
    }

    map->elements = malloc(INITIAL_SIZE * sizeof(Element));
    if (map->elements == NULL)
    {
        free(map);
        return NULL;
    }

    map->size = 0;
    map->maxSize = INITIAL_SIZE;

    return map;
}

void mapDestroy(Map map)
{
    if (map == NULL)
    {
        return;
    }

    while (mapGetSize(map) > 0)
    {
        mapRemove(map, mapGetFirst(map));
    }

    free(map->elements);
    free(map);
}

Map mapCopy(Map map)
{
    if (map == NULL)
    {
        return NULL;
    }

    Map map_copy = malloc(sizeof(*map_copy));
    if (map_copy == NULL)
    {
        return NULL;
    }

    map_copy->size = mapGetSize(map);
    map_copy->maxSize = map->maxSize;

    map_copy->elements = malloc(sizeof(Element) * (map_copy->maxSize));
    if (map_copy->elements == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < mapGetSize(map); i++)
    {
        map_copy->elements[i] = map->elements[i]; //TODO: maybe should copy here
    }

    return map_copy;
}

int mapGetSize(Map map)
{
    if (map == NULL)
    {
        return -1;
    }

    return map->size;
}

bool mapContains(Map map, const char* key)
{
    if (map == NULL || key == NULL)
    {
        return false;
    }

    for (int i = 0; i < mapGetSize(map); i++)
    {
        if (strcmp(map->elements[i]->key, key) == 0)
        {
            return true;
        }
    }

    return false;
}

MapResult mapPut(Map map, const char* key, const char* data)
{
    if (map == NULL || key == NULL || data == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    for (int i = 0; i < mapGetSize(map); i++)
    {
        if (strcmp(map->elements[i]->key, key) == 0)
        {
            char* newData = copyString(data);
            if (newData == NULL)
            {
                return MAP_OUT_OF_MEMORY;
            }

            map->elements[i]->value = newData;
            return MAP_SUCCESS;
        }
    }

    if (mapGetSize(map) == map->maxSize && expand(map) == MAP_OUT_OF_MEMORY) // Resize map if full
    {
        return MAP_OUT_OF_MEMORY;
    }

    /* If the key doesn't exist - add a new a key and assign a new value to it */
    char* newKey = copyString(key);
    char* newData = copyString(data);
    if (newData == NULL || newKey == NULL)
    {
        return MAP_OUT_OF_MEMORY;
    }

    Element element = malloc(sizeof(*element)); // Create element
    if (element == NULL)
    {
        return MAP_OUT_OF_MEMORY; // TODO - check if MAP_ERROR
    }

    element->key = newKey;
    element->value = newData;

    map->elements[mapGetSize(map)] = element;
    map->size++;

    return MAP_SUCCESS;
}

char* mapGet(Map map, const char* key)
{
    if (map == NULL || key == NULL || mapGetSize(map) == 0)
    {
        return NULL;
    }

    for (int i = 0; i < mapGetSize(map); i++)
    {
        if (strcmp(map->elements[i]->key, key) == 0)
        {
            return map->elements[i]->value;
        }
    }

    return NULL;
}

MapResult mapRemove(Map map, const char* key)
{
    if (map == NULL || key == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    for (int i = 0; i < mapGetSize(map); i++)
    {
        if (strcmp(map->elements[i]->key, key) == 0)
        {
            free(map->elements[i]->key);
            free(map->elements[i]->value);

            map->elements[i] = map->elements[map->size - 1];
            map->size--;

            return MAP_SUCCESS;
        }
    }

    return MAP_ITEM_DOES_NOT_EXIST;
}

char* mapGetFirst(Map map)
{
    if (map == NULL || mapGetSize(map) == 0)
    {
        return NULL;
    }

    map->iterator = 0;
    return mapGetNext(map);
}

char* mapGetNext(Map map)
{
    if (map == NULL || map->iterator >= mapGetSize(map))
    {
        return NULL;
    }

    return map->elements[map->iterator++]->key;
}

MapResult mapClear(Map map)
{
    if (map == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    for (int i = 0; i < mapGetSize(map); i++)
    {
        free(map->elements[i]->key);
        free(map->elements[i]->value);
    }
    return MAP_SUCCESS;
}

static MapResult expand(Map map)
{
    int newSize = EXPAND_FACTOR * map->maxSize;

    Element* newElements = realloc(map->elements, newSize * sizeof(newElements));
    if (newElements == NULL)
    {
        return MAP_OUT_OF_MEMORY;
    }

    map->elements = newElements;
    map->maxSize = newSize;

    return MAP_SUCCESS;
}

static char* copyString(const char* str)
{
    char* newStr = malloc(strlen(str) + 1);
    if (newStr == NULL)
    {
        return NULL;
    }

    return strcpy(newStr, str);
}