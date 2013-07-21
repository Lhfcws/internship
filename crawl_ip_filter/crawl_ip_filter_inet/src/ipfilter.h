/**
 * @description IPFilter: Filter crawl ip by a ip dictionary, using Trie.
 * @author Lhfcws Wu （宸风）
 * @time 2013-07-17
 */

#ifndef IPFILTER_H
#define IPFILTER_H

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

typedef unsigned char byte;
// const unsigned int HASH_LMT = 131101;
const unsigned int HASH_LMT = 30000109;
// const long long HASH_LMT = 1024 * 1024 * 1024 * 4;
//
const unsigned int INTMX = (255 << 24) + (255 << 16) + (255 << 8) + 255;



// Link list node
struct IPNode{
    IPNode* next;
    long long ip;

    IPNode(unsigned int _ip):next(NULL){
        this->ip = _ip;
    }
};


// IPFiltes
class IPFilter{
    private:
        IPNode** bucket;
    
        void splitIP(char* raw, char* pch);

        unsigned int Hash(char* str);
        void buildHashChar(char* ip);
        void buildHash(unsigned int val);
        bool findHash(char* ip);
    public:
        IPFilter();
        ~IPFilter();
        void load(const char* filename);
        bool process(char* url);

};


#endif
