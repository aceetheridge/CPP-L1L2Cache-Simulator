#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <sstream>

using namespace std;

int argument_to_int(char *arg[]);
long long get_tag(char addr[], int, int, int);
int get_index(char addr[], int, int, int);
long long address_to_long(char addr[]);
int get_LRU(int indx, int lru[]);
string GetBinaryStringFromHexString(string);


int blockSize;
int cacheSize;
int maxNumberOfBlocks;
int tagAddressLength;
int indx;
int indexLen;
int offsetLen;

int main(int argc, char *argv[]){
    FILE *pfin;
    const char *mode = "r";
    FILE *pfout;
    long int i =0;
    int hit = 0;
    int miss = 0;
    
    int lru[4096][4];
    int lruInstr[4096][4];

    long long addrInLong = 0;
    int op;
    char address[8];
// -c -32 -8 -4
    blockSize = argument_to_int(&argv[3]); // 8 32 128
    cacheSize = argument_to_int(&argv[2]) * 1024; // 32
    char cacheBombinedSeparated = *argv[1]; // c or s
    char assoc = *argv[4]; // 1 or 4
    
    tagAddressLength = 0;
    maxNumberOfBlocks = (int)(cacheSize / blockSize);

    long long cacheBlockTag[4096][4];
    long long cacheTagInstr[4096][4];
    long long requiredTag;
    int limit = 1;
    
    int sc = 0;
    int lc =0;
    int mc =0;
    int flag =0;
    char hitORmiss = 'm';
    int lru_index= 0;

    int addressLen = 0;
    for(lc = 0; lc <4096; lc++){
        for(mc = 0; mc < 4; mc++){
            cacheBlockTag[lc][mc] = 0xffffffff;
            cacheTagInstr[lc][mc] = 0xffffffff;
            lru[lc][mc] = 0;
        }
    }
    if(assoc == '4'){
        limit = 4;
    } 
    else{
        limit = 1;
    }
    offsetLen = (int)((float)log(blockSize)/log(2));

    if(limit == 1){
        indexLen = (int)((float)log(maxNumberOfBlocks)/log(2));
    }
    else{
        indexLen = (int)((float)log(maxNumberOfBlocks)/log(2))-2;
    }
    tagAddressLength = 32 -(indexLen + offsetLen);
    pfin = fopen("trace.din", mode);
    if (pfin == NULL){
        cout << "Can't open file" << endl;
        return(0);
    }
    pfout = fopen("out.txt","w");
    while(fscanf(pfin, "%d %s", &op,address) != EOF){
        addressLen = sizeof(address);
        indx = get_index(address, indexLen, tagAddressLength, offsetLen);
        requiredTag = get_tag(address, tagAddressLength,indexLen, offsetLen);
        addrInLong = address_to_long(address);

        if(cacheBombinedSeparated = 'c' || ((cacheBombinedSeparated = 's') && (op != 2))){
            for (lc = 0; lc < limit; lc++){
                flag = 0;
                if(requiredTag == cacheBlockTag[indx][lc]){
                    flag =1;
                    hit++;
                    hitORmiss = 'h';
                    lc = limit;
                }
                else{
                    flag = 0;
                }
            }
            if(flag == 0){
                miss++;
                hitORmiss = 'm';
                if(limit = 1){
                    cacheBlockTag[indx][0] = requiredTag;
                }
                else{
                    lru_index = get_LRU(indx,lru[indx]);
                    cacheBlockTag[indx][lru_index] = requiredTag;
                }
            }
        }
        else{
            for(lc = 0; lc < limit; lc++){
                flag = 0;
                if(requiredTag == cacheTagInstr[indx][lc]){
                    flag = 1;
                    hit++;
                    hitORmiss = 'h';
                    lc = limit;
                }
                else{
                    flag = 0;
                }
            }
            if(flag == 0){
                miss++;
                hitORmiss = 'm';
                if(limit == 1){
                    cacheTagInstr[indx][0] =requiredTag;
                }
                else{
                    lru_index = get_LRU(indx,lruInstr[indx]);
                    cacheTagInstr[indx][lru_index] = requiredTag;
                }
            }
        }
        i++;
        fprintf(pfout, "%d %s  %d   %lld    %lld    %c \n",op,address,indx,requiredTag,addrInLong,hitORmiss);
    }
    printf("number of Requrests: %ld \n", i);
    fclose(pfout);
    fclose(pfin);
    float percent = hit/i * 100;
    printf("Tag length %d and hits = %d \n\n",miss,hit);
    return 0;
}

int argument_to_int(char *arg[]){
    int result = 0;
    result = (atoi(arg[0])*(-1)); //posible errors
    return result;
}
long long address_to_long(char addr[]){
    string temp(addr);
    long long addressDec = stoll(temp,nullptr,16);
    return addressDec;
}

long long get_tag(char addr[], int taglen, int indlen, int off){
    string temp(addr);
    GetBinaryStringFromHexString(temp);
    string temp2 = temp.substr(0,taglen);
    long long addressDec = stoll(temp2, nullptr, 2);
    return addressDec;
}
int get_index(char addr[], int indlen, int taglen, int off){
    string temp(addr);
    GetBinaryStringFromHexString(temp);
    string temp2 = temp.substr(taglen-1,indlen);
    int addressDec = stoll(temp2, nullptr, 2);
    return addressDec;
}
int get_LRU(int indx, int lru[]){
    int result = lru[indx];
    return result;

}

string GetBinaryStringFromHexString (string sHex){
	string sReturn = "";
	for (int i = 0; i < sHex.length (); ++i){
		switch (sHex [i]){
			case '0': sReturn.append ("0000"); break;
			case '1': sReturn.append ("0001"); break;
            case '2': sReturn.append ("0010"); break;
			case '3': sReturn.append ("0011"); break;
			case '4': sReturn.append ("0100"); break;
			case '5': sReturn.append ("0101"); break;
			case '6': sReturn.append ("0110"); break;
			case '7': sReturn.append ("0111"); break;
			case '8': sReturn.append ("1000"); break;
			case '9': sReturn.append ("1001"); break;
			case 'a': sReturn.append ("1010"); break;
			case 'b': sReturn.append ("1011"); break;
			case 'c': sReturn.append ("1100"); break;
			case 'd': sReturn.append ("1101"); break;
			case 'e': sReturn.append ("1110"); break;
			case 'f': sReturn.append ("1111"); break;
			}
		}
			return sReturn;
	}



