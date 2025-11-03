#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_MASK 32
// #define DEBUG
#ifdef DEBUG
    #define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif



void removeMem();
int add(unsigned int base, char mask);
int del(unsigned int base, char mask);
char check(unsigned int ip);
