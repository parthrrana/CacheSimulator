#ifndef __FIRST_H
#define __FIRST_H

typedef struct Cache{
	int validBit;
	unsigned long tag;
	unsigned long time;
}Cache;

Cache** cacheToUse;
unsigned long memRCacheM = 0;
unsigned long memWrite = 0;
unsigned long cacheHit = 0;
unsigned long timeCounter = 0;

void goToCache(unsigned long, unsigned long, unsigned long, char*, char);

#endif
