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
#include <typeinfo>

using namespace std;

int blockSize;
int cacheSize;
int maxNumberOfBlocks;
int tagAddressLength;
int indx;
int indexLen;
int offsetLen;
int seperation;
int assoc;
double percent;

int argument_to_int(char *arg[]);
string GetBinaryStringFromHexString(string);
int getIndex(string, int, int, int);
unsigned long long getTag(string, int, int ,int);
int get_LRU(int, int lru[]);


int main(int argc, char *argv[]) {
    unsigned long int i;
    unsigned long int hit;
    unsigned long int miss;

    int lru[4096][4];
    int lruInstr[4096][4];

    unsigned long long cacheBlockTag[4096][4];
    unsigned long long cacheTagInstr[4096][4];
    unsigned long long requiredTag;

    int lc =0;
    int mc =0;
    int flag =0;
    char hitORmiss = 'm';
    int lru_index= 0;
    for(lc = 0; lc <4096; lc++){
        for(mc = 0; mc < 4; mc++){
            cacheBlockTag[lc][mc] = 0xffffffff;
            cacheTagInstr[lc][mc] = 0xffffffff;
            lru[lc][mc] = 0;
        }
    }

    blockSize = argument_to_int(&argv[3]); // 8 32 128
    cacheSize = argument_to_int(&argv[2]) * 1024; // 32
    seperation = argument_to_int(&argv[1]); // 1 or 2
    assoc = argument_to_int(&argv[4]); // 1 or 4
    int operation;
    string data;
    maxNumberOfBlocks = (int)cacheSize/blockSize;
    offsetLen = (int)((float)log(blockSize)/log(2));
    if(assoc == 1){
        indexLen = (int)((float)log(maxNumberOfBlocks)/log(2));
    }
    else{
        indexLen = (int)((float)log(maxNumberOfBlocks)/log(2))-2;
    }
    tagAddressLength = 32 -(indexLen + offsetLen);
    ifstream input;
    ofstream output;
    input.open("testtrace.txt");
    output.open("output.txt");
    while(true){
        input >> operation >> data;
        if (operation == 2){
            data = "00" + data;
        }
        indx = getIndex(data,tagAddressLength,indexLen, offsetLen);
        requiredTag = getTag(data, tagAddressLength, indexLen,offsetLen);
        if (seperation == 1 || ((seperation == 2) && (operation != 2))){
            for (lc = 0; lc < assoc; lc++){
                flag = 0;
                if(requiredTag == cacheBlockTag[indx][lc]){
                    flag =1;
                    hit++;
                    hitORmiss = 'h';
                    lc = assoc;
                }
                else{
                    flag = 0;
                }
            }
            if(flag == 0){
                miss++;
                hitORmiss = 'm';
                if(assoc = 1){
                    cacheBlockTag[indx][0] = requiredTag;
                }
                else{
                    lru_index = get_LRU(indx,lru[indx]);
                    cacheBlockTag[indx][lru_index] = requiredTag;
                }
            }
        i++;
        cout << i << " " <<  hit << " " << miss <<  endl;
        }

        if(input.eof()){
            break;
        }
        
    }
    double percent = (double)hit/i * 100.00;
    cout << percent << endl;
    output << "Cache Size: " << cacheSize << " Associativity: " << assoc << " Block Size: " << blockSize << " Seperationg: " << seperation << endl;
    output << "Instructions: " << i << " " << hit << "<-Hit " << miss << "<-Miss " << "Hit Percentage: " << percent << endl;
    input.close();
    output.close();


}
int getIndex(string indata, int tagLen, int indLen, int offLen ){
    // get index address into decimal
    string temp = GetBinaryStringFromHexString(indata);
    string temp2 = temp.substr(tagLen,indLen);
    int decimal = stoll(temp2, nullptr, 2);
    return decimal;
}
unsigned long long getTag(string indata, int tagLen, int indLen, int offLen){
    //get tag into decimal
    string temp = GetBinaryStringFromHexString(indata);
    string temp2 = temp.substr(0,tagLen);
    long long decimal = stoll(temp2, nullptr, 2);
    return decimal;
}
int argument_to_int(char *arg[]){
    int result = 0;
    result = (atoi(arg[0]))*(-1); //posible errors
    cout << result << endl;
    return result;
}
int get_LRU(int indx, int array[]){
    int iSecret = rand() %4 + 1;

    return iSecret;

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