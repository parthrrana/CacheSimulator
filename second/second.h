#ifndef __SECOND_H
#define __SECOND_H

typedef struct Cache{
	int validBit;
	unsigned long tag;
	unsigned long L2Set;
	unsigned long L2Tag;
	unsigned long time;
}Cache;

Cache** L1CacheToUse;
Cache** L2CacheToUse;
unsigned long memRL2CacheM = 0;
unsigned long memWrite = 0;
unsigned long L1CacheHit = 0;
unsigned long L1CacheMiss = 0;
unsigned long L2CacheHit = 0;
unsigned long L1TimeCounter;
unsigned long L2TimeCounter;

void goToCache(unsigned long, unsigned long, unsigned long, char*, char, unsigned long, unsigned long, unsigned long, char*);
void checkL2Cache(unsigned long, unsigned long, unsigned long);
void replaceBlockInL2Cache(unsigned long, unsigned long, unsigned long, char*);

#endif
