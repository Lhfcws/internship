/**
 * @description IPFilter class implementation.
 * @author Lhfcws Wu （宸风）
 * @time 2013-07-17
 */


#include "ipfilter.h"


/**
 * IPFilter implementation.
 */
IPFilter::IPFilter() {
    this->bucket = new IPNode*[HASH_LMT];
    //for (unsigned int i = 0; i < HASH_LMT; i++)
    //   this->bucket[i] = NULL;
    memset(this->bucket, 0x00, sizeof(this->bucket));
}

IPFilter::~IPFilter() {
    delete[] this->bucket;
}


// Hash Function
unsigned int IPFilter::Hash(char *str)
{
    unsigned int result;

    result = inet_network(str);

    return result;
}


void IPFilter::buildHashChar(char* ip) {
    unsigned int val = Hash(ip); 
    this->buildHash(val);
}

void IPFilter::buildHash(unsigned int val) {
    unsigned int key = val % HASH_LMT;

    if (NULL == this->bucket[key]) {
        this->bucket[key] = new IPNode(val);
        return;
    }
    else {
        IPNode* now = new IPNode(val);
        now->next = this->bucket[key];
        this->bucket[key] = now;
    }
}

bool IPFilter::findHash(char* ip) {
    unsigned int val = Hash(ip); 
    unsigned int key = val % HASH_LMT;

    IPNode* now = this->bucket[key];
    
    if (NULL == now)
        return false;

    while (now != NULL) {
        if (now->ip == val)
            return true;
        now = now->next;
    
    }

    return false;
}

// process ip with mask
void IPFilter::splitIP(char* raw, char* pch) {
    byte LEN = pch - raw;
    pch = pch + 1;
    unsigned int mask = atoi(pch);
    raw[LEN] = 0x00;
    

    if (32 == mask) {
        this-> buildHashChar(raw);
        return;
    }

    unsigned int MS = ( INTMX >> (32 - mask) ) << (32 - mask);
    unsigned int ME = (INTMX >> mask);

    unsigned int start = inet_network(raw) & MS;
    unsigned int end = start | ME;
    
    for (unsigned int i = start; i <= end; i++)
        this->buildHash(i);
}

// Public interface
//
// Load & build hash index
void IPFilter::load(const char* filename) {
    
    FILE* fp;
    char line[30];

    fp = fopen(filename, "r");

    while (fgets(line, 30, fp)) {

        line[strlen(line) - 1] = 0x00;
        
        char* pch;
        pch = strchr(line, '/');

        if (NULL == pch)
            this->buildHashChar(line);
        else {
            this->splitIP(line, pch);
        }
    }

    fclose(fp);

}

// Process query
bool IPFilter::process(char* url) {
    char* pch = strstr(url, "ip=");

    if (NULL == pch)
        return true;

    pch = pch + 3;
    char* pch1 = strchr(pch, '&');
    bool bo;

    if (NULL != pch1) {
        int len = pch1 - pch;
        char* ptr = new char[len];
        strncpy(ptr, pch, len);

        bo = this->findHash(ptr);
    }
    else
        bo = this->findHash(pch);

    return !bo;
}
