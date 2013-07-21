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

#include <vector>
#include <string>
#include <sstream>

using namespace std;

typedef unsigned char byte;
// const unsigned int HASH_LMT = 131101;
const unsigned int HASH_LMT = 30000109;
// const unsigned int HASH_LMT = 4000000000;



// Link list node
struct IPNode{
    IPNode* next;
    char* ip;

    IPNode(char* _ip):next(NULL){
        ip = new char[strlen(_ip)];
        strcpy(ip, _ip);
    }
};


// IPFilter
class IPFilter{
    private:
        IPNode** bucket;
    
 //       void splitIP(char* raw);

        unsigned int BKDRHash(char* dtr);
        void buildHash(char* ip);
        bool findHash(char* ip);
    public:
        IPFilter();
        ~IPFilter();
        void load(const char* filename);
        bool process(char* url);

};


#endif
