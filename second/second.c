#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "second.h"

int main(int argc, char* argv[argc+1]){
	if(argc < 9){
		printf("error\n");
		return EXIT_SUCCESS;
	}

	unsigned long L1CacheSize = atoi(argv[1]);
	if(L1CacheSize % 2 != 0){
		printf("error\n");
		return EXIT_SUCCESS;
	}

	char* L1Associativity = argv[2];
	int L1TypeCache;
	unsigned long L1HowManyWay;
	if(strcmp(L1Associativity, "direct") == 0){
		L1TypeCache = 1;
	}
	else if(strcmp(L1Associativity, "assoc") == 0 && strlen(L1Associativity) < 6){
		L1TypeCache = 2;
	}
	else if(strlen(L1Associativity) < 8){
		L1TypeCache = 3;
		L1HowManyWay = L1Associativity[6] - '0';
		if(L1HowManyWay % 2 != 0){
			printf("error\n");
			return EXIT_SUCCESS;
		}
	}

	char* L1CachePolicy = argv[3];

	unsigned long blockSize = atoi(argv[4]);

	unsigned long L1Assoc;
	unsigned long L1NumOfSet;
	if(L1TypeCache == 1){
		L1Assoc = 1;
		L1NumOfSet = L1CacheSize / blockSize;
	}
	else if(L1TypeCache == 2){
		L1Assoc = L1CacheSize / blockSize;
		L1NumOfSet = 1;
	}
	else{
		L1Assoc = L1HowManyWay;
		L1NumOfSet = (L1CacheSize / blockSize) / L1HowManyWay;
	}
	
	L1CacheToUse = malloc(L1NumOfSet * sizeof(Cache));
	for(long i = 0; i < L1NumOfSet; i++){
		L1CacheToUse[i] = malloc(L1Assoc * sizeof(Cache));
		for(long j = 0; j < L1Assoc; j++){
			L1CacheToUse[i][j].validBit = 0;
			L1CacheToUse[i][j].tag = 0;
			L1CacheToUse[i][j].L2Set = 0;
			L1CacheToUse[i][j].L2Tag = 0;
			L1CacheToUse[i][j].time = 0;
		}
	}
	
	unsigned long L2CacheSize = atoi(argv[5]);
	if(L2CacheSize % 2 != 0){
		printf("error\n");
		return EXIT_SUCCESS;
	}
	
	char* L2Associativity = argv[6];
	int L2TypeCache;
	unsigned long L2HowManyWay;
	if(strcmp(L2Associativity, "direct") == 0){
		L2TypeCache = 1;
	}
	else if(strcmp(L2Associativity, "assoc") == 0 && strlen(L2Associativity) < 6){
		L2TypeCache = 2;
	}
	else if(strlen(L2Associativity) < 8){
		L2TypeCache = 3;
		L2HowManyWay = L2Associativity[6] - '0';
		if(L2HowManyWay % 2 != 0){
			printf("error\n");
			return EXIT_SUCCESS;
		}
	}

	char* L2CachePolicy = argv[7];

	unsigned long L2Assoc; 
	unsigned long L2NumOfSet;
	if(L2TypeCache == 1){
		L2Assoc = 1;
		L2NumOfSet = L2CacheSize / blockSize;
	}
	else if(L2TypeCache == 2){
		L2Assoc = L2CacheSize / blockSize;
		L2NumOfSet = 1;
	}
	else{
		L2Assoc = L2HowManyWay;
		L2NumOfSet = (L2CacheSize / blockSize) / L2HowManyWay;
	}
	
	L2CacheToUse = malloc(L2NumOfSet * sizeof(Cache));
	for(long i  = 0; i < L2NumOfSet; i++){
		L2CacheToUse[i] = malloc(L2Assoc * sizeof(Cache));
		for(long j = 0; j < L2Assoc; j++){
			L2CacheToUse[i][j].validBit = 0;
			L2CacheToUse[i][j].tag = 0;
			L2CacheToUse[i][j].L2Set = 0;
			L2CacheToUse[i][j].L2Tag = 0;
			L2CacheToUse[i][j].time = 0;
		}
	}
	
	FILE* fileTrace = fopen(argv[8], "r");
	if(fileTrace == 0){
		printf("error\n");
		return EXIT_SUCCESS;
	}

	unsigned long numBlockOffset = log(blockSize) / log(2);
	unsigned long L1NumSetIndex = log(L1NumOfSet) / log(2);
	unsigned long L2NumSetIndex = log(L2NumOfSet) / log(2);

	unsigned long L1SetIndex;
	unsigned long L1TagAddress;
	unsigned long L2SetIndex;
	unsigned long L2TagAddress;
	char readOrWrite;
	unsigned long address;

	while(fscanf(fileTrace, "%c %lx\n", &readOrWrite, &address) != EOF){
		L1SetIndex = (address >> numBlockOffset) & ((1LU << L1NumSetIndex) - 1);
		L1TagAddress = address >> (numBlockOffset + L1NumSetIndex);
		L2SetIndex = (address >> numBlockOffset) & ((1LU << L2NumSetIndex) - 1);
		L2TagAddress = address >> (numBlockOffset + L2NumSetIndex);

		goToCache(L1SetIndex, L1TagAddress, L1Assoc, L1CachePolicy, readOrWrite, L2SetIndex, L2TagAddress, L2Assoc, L2CachePolicy);
	}
	
	fclose(fileTrace);

	printf("memread:%lu\n", memRL2CacheM);
	printf("memwrite:%lu\n", memWrite);
	printf("l1cachehit:%lu\n", L1CacheHit);
	printf("l1cachemiss:%lu\n", L1CacheMiss);
	printf("l2cachehit:%lu\n", L2CacheHit);
	printf("l2cachemiss:%lu\n", memRL2CacheM);

	/*free cache*/
	for(long i = 0; i < L1NumOfSet; i++){
		free(L1CacheToUse[i]);
	}
	free(L1CacheToUse);
	
	for(long i = 0; i < L2NumOfSet; i++){
		free(L2CacheToUse[i]);
	}
	free(L2CacheToUse);

	return EXIT_SUCCESS;
}

void goToCache(unsigned long L1SetIndex, unsigned long L1TagAddress, unsigned long L1Assoc, char* L1CachePolicy, char readOrWrite, unsigned long L2SetIndex, unsigned long L2TagAddress, unsigned long L2Assoc, char* L2CachePolicy){
	unsigned long counter = 0;
	while(counter != L1Assoc){
		if(L1CacheToUse[L1SetIndex][counter].validBit == 0){
			L1CacheMiss++;
			if(readOrWrite == 'W'){
				memWrite++;
			}
			checkL2Cache(L2SetIndex, L2TagAddress, L2Assoc);
			L1CacheToUse[L1SetIndex][counter].validBit = 1;
			L1CacheToUse[L1SetIndex][counter].tag = L1TagAddress;
			L1CacheToUse[L1SetIndex][counter].L2Set = L2SetIndex;
			L1CacheToUse[L1SetIndex][counter].L2Tag = L2TagAddress;
			L1TimeCounter++;
			L1CacheToUse[L1SetIndex][counter].time = L1TimeCounter;
			return;
		}
		else if(L1CacheToUse[L1SetIndex][counter].validBit == 1 && L1CacheToUse[L1SetIndex][counter].tag == L1TagAddress){
			L1CacheHit++;
			if(readOrWrite == 'W'){
				memWrite++;
			}
			if(strcmp(L1CachePolicy, "lru") == 0){
				L1TimeCounter++;
				L1CacheToUse[L1SetIndex][counter].time = L1TimeCounter;
			}
			return;
		}
		counter++;
	}

	L1CacheMiss++;
	if(readOrWrite == 'W'){
		memWrite++;
	}
	checkL2Cache(L2SetIndex, L2TagAddress, L2Assoc);
	unsigned long blockToReplace = 0;
	unsigned long blockCounter = 0;
	if(strcmp(L1CachePolicy, "fifo") == 0){
		while(blockCounter != L1Assoc){
			if(L1CacheToUse[L1SetIndex][blockCounter].time < L1CacheToUse[L1SetIndex][blockToReplace].time){
				blockToReplace = blockCounter;
			}
			blockCounter++;
		}
	}
	else{
		while(blockCounter != L1Assoc){
			if(L1CacheToUse[L1SetIndex][blockToReplace].time > L1CacheToUse[L1SetIndex][blockCounter].time){
				blockToReplace = blockCounter;
			}
			blockCounter++;
		}
	}
	
	unsigned long replaceBlockL2Set = L1CacheToUse[L1SetIndex][blockToReplace].L2Set;
	unsigned long replaceBlockL2Tag = L1CacheToUse[L1SetIndex][blockToReplace].L2Tag;
	replaceBlockInL2Cache(replaceBlockL2Set, replaceBlockL2Tag, L2Assoc, L2CachePolicy);	

	L1CacheToUse[L1SetIndex][blockToReplace].validBit = 1;
	L1CacheToUse[L1SetIndex][blockToReplace].tag = L1TagAddress;
	L1CacheToUse[L1SetIndex][blockToReplace].L2Set = L2SetIndex;
	L1CacheToUse[L1SetIndex][blockToReplace].L2Tag = L2TagAddress;
	L1TimeCounter++;
	L1CacheToUse[L1SetIndex][blockToReplace].time = L1TimeCounter;
	return;
}

void checkL2Cache(unsigned long L2SetIndex, unsigned long L2TagAddress, unsigned long L2Assoc){
	unsigned long counter = 0;
	while(counter != L2Assoc){
		if(L2CacheToUse[L2SetIndex][counter].validBit == 0){
			memRL2CacheM++;
			return;
		}
		else if(L2CacheToUse[L2SetIndex][counter].validBit == 1 && L2CacheToUse[L2SetIndex][counter].L2Tag == L2TagAddress){
			L2CacheHit++;
			L2CacheToUse[L2SetIndex][counter].validBit = 0;
			L2CacheToUse[L2SetIndex][counter].L2Tag = 0;
			//L2TimeCounter++;
			L2CacheToUse[L2SetIndex][counter].time = 0;
			return;
		}
		counter++;
	}
	memRL2CacheM++;
	return;
}

void replaceBlockInL2Cache(unsigned long L2SetIndex, unsigned long L2TagAddress, unsigned long L2Assoc, char* L2CachePolicy){
	unsigned long counter = 0;
	while(counter != L2Assoc){
		if(L2CacheToUse[L2SetIndex][counter].validBit == 0){
			L2CacheToUse[L2SetIndex][counter].validBit = 1;
			L2CacheToUse[L2SetIndex][counter].L2Tag = L2TagAddress;
			L2TimeCounter++;
			L2CacheToUse[L2SetIndex][counter].time = L2TimeCounter;
			return;
		}
		counter++;
	}

	unsigned long blockToReplace = 0;
	unsigned long blockCounter = 0;
	if(strcmp(L2CachePolicy, "fifo") == 0){
		while(blockCounter != L2Assoc){
			if(L2CacheToUse[L2SetIndex][blockCounter].time < L2CacheToUse[L2SetIndex][blockToReplace].time){
				blockToReplace = blockCounter;
			}
			blockCounter++;
		}
	}
	else{
		while(blockCounter != L2Assoc){
			if(L2CacheToUse[L2SetIndex][blockToReplace].time > L2CacheToUse[L2SetIndex][blockCounter].time){
				blockToReplace = blockCounter;
			}
			blockCounter++;
		}
	}
	L2CacheToUse[L2SetIndex][blockToReplace].validBit = 1;
	L2CacheToUse[L2SetIndex][blockToReplace].L2Tag = L2TagAddress;
	L2TimeCounter++;
	L2CacheToUse[L2SetIndex][blockToReplace].time = L2TimeCounter;
	return;
}
