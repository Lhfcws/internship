/**
 * @description IPFilter: Filter crawl ip by a ip dictionary, using Trie.
 * @author Lhfcws Wu （宸风）
 * @time 2013-07-16
 */

#ifndef IPFILTER_H
#define IPFILTER_H

#include "ctype.h"
#include "string.h"
#include "stdlib.h"
#include <string>

class Trie{
public:
    Trie* next[2];
    int val;
    bool end;

    Trie() {
        this->next[0] = NULL;
        this->next[1] = NULL;
        this->val = -1;
        this->end = false;
    }
    ~Trie() {
        delete this->next[0];
        delete this->next[1];
    }
};

class IP{
public:
    int mask;
    char ip[40];
    IP(char* raw);
};

class IPFilter {
    private:
        Trie* root;
        char* success;

        /**
         * Build trie
         */
        void buildTrie(IP ipobj);
        /**
         * Find IP from url.
         */
        void findIP(char* url, char* ip);
        /**
         * Test a IP if it is crawl.
         */
        bool test(char* raw);
    public:
        IPFilter();
        ~IPFilter();
        void load(char* filename);
        bool process(const char* url);
};

#endif

