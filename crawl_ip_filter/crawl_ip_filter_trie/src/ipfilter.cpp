/**
 * @description IPFilter class implementation.
 * @author Lhfcws Wu （宸风）
 * @time 2013-07-16
 */
#include "ipfilter.h"
#include "stdio.h"
#include <string>

std::string toBinary(const char* decNumber) {
    int cnt = 7;
    int number = atoi(decNumber);
    std::string bin = "00000000";

    while (number > 0) {
        bin[cnt] = (char)(number % 2 + 48);
        cnt--;
        number /= 2;
    }

    return bin;
}

/**
 * IP implementation.
 */
 
IP::IP(char* raw) {
    this->mask = 32;
    this->ip[0] = 0x00;
    std::string number = "";
    bool hasMask = false;

    for (unsigned i = 0; i < strlen(raw); i++) {
        if (isdigit(raw[i])) {
            number = number + raw[i];               
        }       
        if (!hasMask && number != "" && (!isdigit(raw[i]) || '.' == raw[i])) {
            std::string sr = toBinary(number.c_str());
            strcat(this->ip, sr.c_str());
            this->mask = strlen(this->ip);
            number = "";
        }
        if ('/' == raw[i]) {
            hasMask = true;
            continue;
        }
    }


    if (hasMask) {
        this->mask = atoi(number.c_str());
    }

    this->ip[this->mask] = 0x00;
}

/**
 * IPFilter implementation
 */
IPFilter::IPFilter() {
    this->root = new Trie();
    this->success = "pass";
}

IPFilter::~IPFilter() {
    delete this->root;   
}

void IPFilter::buildTrie(IP ipobj) {
    

    int i = 0;
    Trie* now = this->root;

    while (i < strlen(ipobj.ip)) {
        int key = (int)(ipobj.ip[i] - 48);
        
        if (NULL == now->next[key]) {
            now->next[key] = new Trie();
            now->next[key]->val = key;
        }

        now = now->next[key];
        i++;
    }
    now->end = true;
}

void IPFilter::findIP(char* url, char* ip) {
    char* ipChar = "ip=";
    char* pch = strstr(url, ipChar);

    if (NULL == pch) {
        strcpy(ip, this->success);
        return;
    }

    int cnt = 0;
    for (unsigned i = 3; i < strlen(pch); i++) {
        if ('&' == pch[i])     
            break;

        ip[cnt] = pch[i];
        cnt++;
    }
    ip[cnt] = 0x00;     // 截断长度

}


bool IPFilter::test(char* raw) {
    IP ipobj(raw);
    Trie* now = this->root;
    int i = 0;

    while (i <= strlen(ipobj.ip)) {
        int key = (int)(ipobj.ip[i] - 48);
        if (NULL == now)
            break;
        if (now->end) {
            return false;
        }
        now = now->next[key];
        i++;
    }

    return true;
}

void IPFilter::load(char* filename) {
    FILE* fp;
    char line[30];

    fp = fopen(filename, "r");

    while (fgets(line, 30, fp)) {
        if (NULL == line)
            continue;

        line[strlen(line) - 1] = 0x00;
        IP ipobj(line); 
        this->buildTrie(ipobj);
    }
    fclose(fp);
}


bool IPFilter::process(const char* url) {
    char ip[20];
    char buffer[1600];
    strcpy(buffer, url);
    this->findIP(buffer, ip);
    if (this->success[0] == ip[0]) {
        return true;
    }

    bool success = this->test(ip);
    if (success) {
        return true;
    }
    else
        return false;
}
