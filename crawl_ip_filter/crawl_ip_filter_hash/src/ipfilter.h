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
const unsigned int HASH_LMT = 131101;
// const unsigned int HASH_LMT = 30000109;
// const unsigned int HASH_LMT = 4000000000;



// IP
class IP{
    public:
        unsigned int ip[4];
        unsigned int __hash__;

        IP();
        IP(const char* raw, byte begin);
        IP(const IP& ipobj);

        void push(int val);
        void loadString(const char* raw, byte begin);
        unsigned int hash();
        bool equal(const IP& ipobj);
};

// Link list node
struct IPNode{
    IP ipobj;
    IPNode* next;

    IPNode(const IP& _ipobj):ipobj(_ipobj), next(NULL){}
};


// IPFilter
class IPFilter{
    private:
        IPNode** bucket;
        vector<string> vecs;
    
        void generateIP(string ipsr, byte dep, byte start, byte end);
        void splitIP(const char* raw);

        unsigned int hash(const IP& ipobj);
        void buildHash(const IP& ipobj);
        bool findHash(const IP& ipobj);
    public:
        IPFilter();
        ~IPFilter();
        void load(const char* filename);
        bool process(const char* url);

};


#endif
