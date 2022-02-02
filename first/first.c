#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "first.h"

int main(int argc, char* argv[argc+1]){
	if(argc < 6){
		printf("error\n");
		return EXIT_SUCCESS;
	}

	unsigned long cacheSize = atoi(argv[1]);
	if(cacheSize % 2 != 0){
		printf("error\n");
		return EXIT_SUCCESS;
	}

	char* associativity = argv[2];
	int typeCache;
	unsigned long howManyWay;
	if(strcmp(associativity, "direct") == 0){
		typeCache = 1;
	}
	else if(strcmp(associativity, "assoc") == 0 && strlen(associativity) < 6){
		typeCache = 2;
	}
	else if(strlen(associativity) < 8){
		typeCache = 3;
		howManyWay = associativity[6] - '0';
		if(howManyWay % 2 != 0){
			printf("error\n");
			return EXIT_SUCCESS;
		}
	}

	char* cachePolicy = argv[3];

	unsigned long blockSize = atoi(argv[4]);

	unsigned long assoc;
	unsigned long numOfSet;
	if(typeCache == 1){
		assoc = 1;
		numOfSet = cacheSize / blockSize;
	}
	else if(typeCache == 2){
		assoc = cacheSize / blockSize;
		numOfSet = 1;
	}
	else{
		assoc = howManyWay;
		numOfSet = (cacheSize / blockSize) / howManyWay;
	}
	
	cacheToUse = malloc(numOfSet * sizeof(Cache));
	for(long i = 0; i < numOfSet; i++){
		cacheToUse[i] = malloc(assoc * sizeof(Cache));
		for(long j = 0; j < assoc; j++){
			cacheToUse[i][j].validBit = 0;
			cacheToUse[i][j].tag = 0;
			cacheToUse[i][j].time = 0;
		}
	}

	FILE* fileTrace = fopen(argv[5], "r");
	if(fileTrace == 0){
		printf("error\n");
		return EXIT_SUCCESS;
	}

	unsigned long numBlockOffset = log(blockSize) / log(2);
	unsigned long numSetIndex = log(numOfSet) / log(2);

	unsigned long setIndex;
	unsigned long tagAddress;
	char readOrWrite;
	unsigned long address;

	while(fscanf(fileTrace, "%c %lx\n", &readOrWrite, &address) != EOF){
		setIndex = (address >> numBlockOffset) & ((1LU << numSetIndex) - 1);
		tagAddress = address >> (numBlockOffset + numSetIndex);

		goToCache(setIndex, tagAddress, assoc, cachePolicy, readOrWrite);
	}
	
	fclose(fileTrace);

	printf("memread:%lu\n", memRCacheM);
	printf("memwrite:%lu\n", memWrite);
	printf("cachehit:%lu\n", cacheHit);
	printf("cachemiss:%lu\n", memRCacheM);

	/*free cache*/
	for(long i = 0; i < numOfSet; i++){
		free(cacheToUse[i]);
	}
	free(cacheToUse);

	return EXIT_SUCCESS;
}

void goToCache(unsigned long setIndex, unsigned long tagAddress, unsigned long assoc, char* cachePolicy, char readOrWrite){
	unsigned long counter = 0;
	while(counter != assoc){
		if(cacheToUse[setIndex][counter].validBit == 0){
			memRCacheM++;
			if(readOrWrite == 'W'){
				memWrite++;
			}
			cacheToUse[setIndex][counter].validBit = 1;
			cacheToUse[setIndex][counter].tag = tagAddress;
			timeCounter++;
			cacheToUse[setIndex][counter].time = timeCounter;
			return;
		}
		else if(cacheToUse[setIndex][counter].validBit == 1 && cacheToUse[setIndex][counter].tag == tagAddress){
			cacheHit++;
			if(readOrWrite == 'W'){
				memWrite++;
			}
			if(strcmp(cachePolicy, "lru") == 0){
				timeCounter++;
				cacheToUse[setIndex][counter].time = timeCounter;
			}
			return;
		}
		counter++;
	}
	memRCacheM++;
	if(readOrWrite == 'W'){
		memWrite++;
	}
	unsigned long blockToReplace = 0;
	unsigned long blockCounter = 0;
	if(strcmp(cachePolicy, "fifo") == 0){
		while(blockCounter != assoc){
			if(cacheToUse[setIndex][blockCounter].time < cacheToUse[setIndex][blockToReplace].time){
				blockToReplace = blockCounter;
			}
			blockCounter++;
		}
	}
	else{
		while(blockCounter != assoc){
			if(cacheToUse[setIndex][blockToReplace].time > cacheToUse[setIndex][blockCounter].time){
				blockToReplace = blockCounter;
			}
			blockCounter++;
		}
	}
	cacheToUse[setIndex][blockToReplace].validBit = 1;
	cacheToUse[setIndex][blockToReplace].tag = tagAddress;
	timeCounter++;
	cacheToUse[setIndex][blockToReplace].time = timeCounter;
	return;
}

