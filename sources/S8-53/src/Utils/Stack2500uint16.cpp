/// @file

#include "Stack2500uint16.h"
#include "Log.h"
#include "Hardware/Timer.h"

/*
static const int NUM_ELEMENTS = 2500;
static uint16 buffer[NUM_ELEMENTS];
static int pFirst = 0;
static int pLast = 0;
static bool empty = true;

static int Size();
static void Ranges(int fromEnd, int *start0, int *end0, int *start1, int *end1);

void Stack_AddValue(uint16 value) {
    if(empty) {   // стек пустой
        buffer[0] = value;
        empty = false;
    } else {
        pLast++;
        if(pLast == NUM_ELEMENTS) {
            pLast = 0;
        }
        buffer[pLast] = value;
        if(pLast == pFirst) {
            pFirst++;
            if(pFirst == NUM_ELEMENTS) {
                pFirst = 0;
            }
        }
    }
}

void Stack_MinMaxLast(int number, uint16 *min, uint16 *max) {
    int range = number;
    int size = Size();
    if(size < range) {
        range = size;
    }
    int start0 = 0;
    int end0 = 0;
    int start1 = 0;
    int end1 = 0;
    Ranges(range, &start0, &end0, &start1, &end1);
    uint16 minV = 0xffff;
    uint16 maxV = 0x0;
    if(start0 != -1) {
        for(int i = start0; i <= end0; i++) {
            uint16 value = buffer[i];
            if(value < minV) {
                minV = value;
            }
            if(value > maxV) {
                maxV = value;
            }
        }
        if(start1 != -1) {
            for(int i = start1; i <= end1; i++) {
                uint16 value = buffer[i];
                if(value < minV) {
                    minV = value;
                }
                if(value > maxV) {
                    maxV = value;
                }
            }
        }
    }
    *min = minV;
    *max = maxV;
}

int Size(void) {
    if(empty) {
        return 0;
    }
    if(pLast > pFirst) {
        return pLast - pFirst + 1;
    }
    return NUM_ELEMENTS;
}

void Ranges(int numElements, int *start0, int *end0, int *start1, int *end1) {
    *start0 = *start1 = -1;
    if(pLast >= pFirst) {
        *start0 = pLast - numElements + 1;
        *end0 = pLast;
    } else if(numElements <= pLast - 1) {
        *start0 = pLast - numElements + 1;
        *end0 = pLast;
    } else {
        *start0 = 0;
        *end0 = pLast;
        *start1 = NUM_ELEMENTS - (numElements - 1 - pLast);
        *end1 = NUM_ELEMENTS - 1;
    }
    
}

*/
