#include "ipLib.h"

typedef struct prefix_s
{
    unsigned int *base;
    char *mask;
    unsigned int iteration;
    bool catchAllExists;
    unsigned int whereIsCatchAll;
}prefix_t;

prefix_t prefix = {.base = NULL, .mask = NULL, .iteration = 0, .catchAllExists = false};

void removeMem()
{
    free(prefix.base);
    free(prefix.mask);
    prefix.base = NULL;
    prefix.mask = NULL;
    prefix.iteration = 0;
    prefix.catchAllExists = false;
    prefix.whereIsCatchAll = 0;
}

void printPrefix()
{
    for(unsigned int i = 0; i < prefix.iteration; i++)
    {
        DEBUG_PRINTF("Prefix: %d.%d.%d.%d/%d\n",
                                                    ((prefix.base[i]) >> 24) & 0xFF, 
                                                    ((prefix.base[i]) >> 16) & 0xFF, 
                                                    ((prefix.base[i]) >> 8) & 0xFF, 
                                                    ((prefix.base[i]) & 0xFF), 
                                                    (prefix.mask[i]));
    }
    if(prefix.catchAllExists)
    {
        DEBUG_PRINTF("Number to prefix with 0 mask: %d\n", prefix.whereIsCatchAll);
    }
}

int add(unsigned int base, char mask)
{
    
    if( ((base >> 24) & 0xFF) > 255 || 
        (((base >> 16) & 0xFF) > 255) || 
        (((base >> 8) & 0xFF) > 255) || 
        ((base & 0xFF) > 255) || 
        (mask > MAX_MASK) || 
        (mask < 0))
    {
        DEBUG_PRINTF("[Error]: Wrong argument");
        return -1;
    }

    if((prefix.catchAllExists) && (mask == 0))
    {
        DEBUG_PRINTF("[Error]: There is a prefix with a zero mask. The prefix with zero mask exists in %d position of database list\n", prefix.whereIsCatchAll);
        return -1;
    }

    if((prefix.base == NULL) && (prefix.mask == NULL))
    {
        prefix.base = (unsigned int*)malloc(sizeof(unsigned int));
        if(prefix.base == NULL)
        {
            DEBUG_PRINTF("[Error]: There was a problem adding first element for base prefix");
            return -2;
        }

        *prefix.base = base;

        prefix.mask = (char*)malloc(sizeof(char));
        if(prefix.mask == NULL)
        {
            DEBUG_PRINTF("[Error]: There was a problem adding first element for mask");
            return -2;
        }

        *prefix.mask = mask;
        if(*prefix.mask == 0)
        {
            prefix.catchAllExists = true;
            prefix.whereIsCatchAll = prefix.iteration;
        }

        prefix.iteration++;

        printPrefix();
        
        return 0;
    }
    else if(((prefix.base == NULL) && (prefix.mask != NULL)) || (prefix.base != NULL) && (prefix.mask == NULL))
    {
        DEBUG_PRINTF("[Error]: There was a problem adding prefix\n");
        return -2;
    }
    else
    {
        
        for(unsigned int i = 0; i < prefix.iteration; i++)
        {
            if((prefix.base[i] == base) && (prefix.mask[i] == mask))
            {
                DEBUG_PRINTF("[Error]: The given arguments exist in the proxy database");
                return -1;
            }
        }

        unsigned int *tmpBase = (unsigned int*)realloc(prefix.base, (prefix.iteration + 1) * sizeof(unsigned int));
        if(tmpBase == NULL)
        {
            DEBUG_PRINTF("[Error]: There was a problem expanding the dynamic table for base value.\n");
            return -2;
        }
        prefix.base = tmpBase;
        
        char *tmpMask = (char*)realloc(prefix.mask, (prefix.iteration + 1) * sizeof(char));
        if(tmpMask == NULL)
        {
            DEBUG_PRINTF("[Error]: There was a problem expanding the dynamic table for mask.\n");
            return -2;
        }
        prefix.mask = tmpMask;

        prefix.base[prefix.iteration] = base;
        prefix.mask[prefix.iteration] = mask;
        if(prefix.mask[prefix.iteration] == 0)
        {
            prefix.catchAllExists = true;
        }
        prefix.iteration++;

        for(unsigned int i = 0; i < prefix.iteration; i++)
        {
            for(unsigned int j = i; j < prefix.iteration; j++)
            {
                if((prefix.base[i] < prefix.base[j]))
                {
                    unsigned int tmpBase = prefix.base[i];
                    prefix.base[i] = prefix.base[j];
                    prefix.base[j] = tmpBase;

                    char tmpMask = prefix.mask[i];
                    prefix.mask[i] = prefix.mask[j];
                    prefix.mask[j] = tmpMask;
                    if(prefix.mask[i] == 0)
                    {
                        prefix.whereIsCatchAll = i;
                    }
                }
                if((prefix.base[i] == prefix.base[j]) && (prefix.mask[i] < prefix.mask[j]))
                {
                    unsigned int tmpBase = prefix.base[i];
                    prefix.base[i] = prefix.base[j];
                    prefix.base[j] = tmpBase;

                    char tmpMask = prefix.mask[i];
                    prefix.mask[i] = prefix.mask[j];
                    prefix.mask[j] = tmpMask;

                    if(prefix.mask[i] == 0)
                    {
                        prefix.whereIsCatchAll = i;
                    }
                }
            }
        }

        printPrefix();

        return 0;
    }

}

int del(unsigned int base, char mask)
{
    if((prefix.base == NULL) && (prefix.mask == NULL))
    {
        DEBUG_PRINTF("[Error]: Prefix base is empty!\n");
        return -1;
    }
    if((prefix.iteration == 1) && (prefix.base[0] == base) && (prefix.mask[0] == mask))
    {
        DEBUG_PRINTF("The proxy database was immediately released!\n");
        removeMem();
        return 0;
    }
    for (unsigned int i = 0; i < prefix.iteration; i++)
    {
        if((prefix.base[i] == base) && (prefix.mask[i] == mask))
        {
            if(prefix.mask[i] == 0)
            {
                prefix.catchAllExists = false;
            }

            for(unsigned int j = i; j < prefix.iteration-1; j++)
            {
                prefix.base[j] = prefix.base[j+1];
                prefix.mask[j] = prefix.mask[j+1];
            }

            unsigned int *tmpBase = (unsigned int*)realloc(prefix.base, prefix.iteration * sizeof(unsigned int));
            if(tmpBase == NULL)
            {
                DEBUG_PRINTF("[Error]: There was a problem reducing the dynamic table for base value.\n");
                return -2;
            }
            prefix.base = tmpBase;

            char *tmpMask =  (char*)realloc(prefix.mask, prefix.iteration * sizeof(char));
            if(tmpMask == NULL)
            {
                DEBUG_PRINTF("[Error]: There was a problem reducing the dynamic table for mask.\n");
                return -2;
            }

            prefix.mask = tmpMask;
            prefix.iteration--;

            printPrefix();
            return 0;
        }
    }

    DEBUG_PRINTF("[Error] Prefix doesn't exists in prefix base\n");
    return -1;
}

char check(unsigned int ip)
{
    DEBUG_PRINTF("IP to check: 0x%x\n", ip);    

    for(int i = 0; i < prefix.iteration; i++)
    {
        unsigned int network = (MAX_MASK - prefix.mask[i]);
        if((prefix.base[i] >> network) == (ip >> network))
        {
            DEBUG_PRINTF("prefix.base[%d]: 0x%x; prefix.mask[%d]: %d\n", i, prefix.base[i], i, prefix.mask[i]);
            unsigned int host = ((1 << network) - 1);
            return prefix.mask[i];
        }
    }

    if(prefix.catchAllExists)
    {
        DEBUG_PRINTF("IP belongs to the prefix with zero mask\n");
        DEBUG_PRINTF("prefix.mask[%d] to return: %d\n", prefix.whereIsCatchAll, prefix.mask[prefix.whereIsCatchAll]);
        return prefix.mask[prefix.whereIsCatchAll];
    }
    DEBUG_PRINTF("[Error] IP doesn't match to prefix base\n");
    return -1;
}