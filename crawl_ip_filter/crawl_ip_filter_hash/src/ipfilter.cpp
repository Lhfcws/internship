/**
 * @description IPFilter class implementation.
 * @author Lhfcws Wu （宸风）
 * @time 2013-07-17
 */

#include <iostream>
#include "ipfilter.h"


string to_string(int num) {
    stringstream ss;
    string str;
    ss << num;
    ss >> str;
    return str;
}

/**
 * IP implementation.
 */
IP::IP() {
    this->__hash__ = -1;
    for (int i=0; i<4; i++) ip[i] = -1;
}

IP::IP(const char* raw, byte begin = 0) {
    for (int i=0; i<4; i++) ip[i] = -1;
    this->loadString(raw, begin);
}

IP::IP(const IP& ipobj) {
    for (byte i = 0; i < 4; i++)
        this->ip[i] = ipobj.ip[i];
    this->__hash__ = ipobj.__hash__;
}

void IP::push(int val) {
    for (int i = 0; i < 4; i++) {
        if (-1 == this->ip[i]) {
            this->ip[i] = val;
            return;
        }
    }
}

void IP::loadString(const char* raw, byte begin = 0) {
    byte i;
    string num;
    for (i = begin; i <= strlen(raw); i++) {
        if (isdigit(raw[i])) {
            num = num + raw[i];
        }
        else {
            int _num = atoi(num.c_str());
            this->push(_num);
            num = "";
        }
    }

    this->hash();
}

unsigned int IP::hash() {
    this->__hash__ = ((ip[0] << 24) + (ip[1] << 16) + (ip[2] << 8) + ip[3]) % HASH_LMT;
    return this->__hash__;
}

bool IP::equal(const IP& ipobj) {
    return  this->ip[0] == ipobj.ip[0] && \
            this->ip[1] == ipobj.ip[1] && \
            this->ip[2] == ipobj.ip[2] && \
            this->ip[3] == ipobj.ip[3] && \
            this->__hash__ == ipobj.__hash__;
}

/**
 * IPFilter implementation.
 */
IPFilter::IPFilter() {
    this->bucket = new IPNode*[HASH_LMT];
    for (int i = 0; i < HASH_LMT; i++)
       this->bucket[i] = NULL;
    // memset(this->bucket, 0x00, sizeof(this->bucket));
}

IPFilter::~IPFilter() {

}

void IPFilter::generateIP(string ipsr, byte dep, byte start, byte end) {
    if (dep > 3) {
        (this->vecs).push_back(ipsr);
        return;
    }

    short i = start;
    for (; i <= end; i++) {
        //if (end == i) {
        //    printf("STOP!!\n");
        //}
        string str = ipsr + "." + to_string(i);
        generateIP(str, dep + 1, 0, 255);
    }
    return;
}

void IPFilter::splitIP(const char* raw) {
    string sr(raw);

    size_t pos = sr.find('/');

    if (string::npos == pos) {
        vecs.push_back(sr);
    }
    else {
        string ipsr = sr.substr(0, pos);            // Get original IP 
        IP ipobj(ipsr.c_str());                         
        int mask = atoi(sr.substr(pos + 1).c_str());    // net mask
        int n = mask / 8;                               // Get static ip segment
        int mod = 8 - mask % 8;                         

        byte mask_l = ((ipobj.ip[n]) >> mod) << mod;                        // mask to the closest dot on the left.
        byte start = mask_l;
        byte end = mask_l | (255 >> (8 - mod));
        
        ipsr.assign("");
        for (int i = 0; i < n; i++) {
            if (i == 0)
                ipsr = ipsr + to_string(ipobj.ip[i]);
            else {
                ipsr = ipsr + ".";
                ipsr = ipsr + to_string(ipobj.ip[i]);
            }
        }

        this->vecs.clear();
        this->generateIP(ipsr, n, start, end);
    }

}

void IPFilter::buildHash(const IP& ipobj) {
    unsigned int key = ipobj.__hash__; 
    if (NULL == this->bucket[key]) {
        this->bucket[key] = new IPNode(ipobj);
        return;
    }
    else {
        IPNode* now = this->bucket[key];
        while (NULL != now->next) {
            now = now->next;
        }

        now->next = new IPNode(ipobj);
    }
}

bool IPFilter::findHash(const IP& ipobj) {
    unsigned int key = ipobj.__hash__;
    IPNode* now = this->bucket[key];
    
    if (NULL == now)
        return false;

    bool bo = false;
    while (now != NULL && !(bo = (now->ipobj).equal(ipobj))) {
        now = now->next;
    
    }

    return bo;
}

// Public interface
//
void IPFilter::load(const char* filename) {
    
    FILE* fp;
    char* line = new char[];

    fp = fopen(filename, "r");

    while (fgets(line, 30, fp)) {
        if (NULL == line)
            continue;
        
        line[strlen(line) - 1] = 0x00;
        printf("%s\n", line);
        
        this->splitIP(line);
        vector<string>::iterator iter;

        for (iter = this->vecs.begin(); iter != this->vecs.end(); iter++) {
            IP ipobj;
            ipobj.loadString(iter->c_str());
            this->buildHash(ipobj);
        }
    }

    fclose(fp);

}

bool IPFilter::process(const char* url) {
    char buffer[1600];
    strcpy(buffer, url);
    char* pch = strstr(buffer, "ip=");

    if (NULL == pch)
        return true;

    IP ipobj(pch, 3);

    return !this->findHash(ipobj);
}
