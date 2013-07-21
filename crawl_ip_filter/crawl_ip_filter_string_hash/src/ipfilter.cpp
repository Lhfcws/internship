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
    for (unsigned int i = 0; i < HASH_LMT; i++)
       this->bucket[i] = NULL;
    // memset(this->bucket, 0x00, sizeof(this->bucket));
}

IPFilter::~IPFilter() {}


// BKDR Hash Function
unsigned int IPFilter::BKDRHash(char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
    {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF) % HASH_LMT;
}

void IPFilter::buildHash(char* ip) {
    unsigned int key = BKDRHash(ip); 
    if (NULL == this->bucket[key]) {
        this->bucket[key] = new IPNode(ip);
        return;
    }
    else {
        IPNode* now = this->bucket[key];
        while (NULL != now->next) {
            now = now->next;
        }

        now->next = new IPNode(ip);
    }
}

bool IPFilter::findHash(char* ip) {
    unsigned int key = BKDRHash(ip); 
    IPNode* now = this->bucket[key];
    
    if (NULL == now)
        return false;

    while (now != NULL) {
        if (0 == strcmp(now->ip, ip))
            return true;
        now = now->next;
    
    }

    return false;
}
/*
void IPFilter::splitIP(char* rawip, char* pch) {
        int len = pch - 1 - rawip;
        char* ip = new char[len];
        strncpy(ip, rawip, len);

        int mask = atoi(ip);    // net mask
        int n = mask / 8;       // Get static ip segment
        int mod = 8 - mask % 8;                         

        int cnt = -1;
        int dot = -1;
        char st[3];
        int cnt_st = 0;

        for (byte i = 0; i < strlen(ip); i++) {
            if (cnt == n - 1 && isdigit(ip[i])) {
                st[cnt_st] = ip[i];
                cnt_st++;
            }
            if ('.' == ip[i]) {
                cnt++;
                dot = i;
            }
            if (cnt == n) {
                st[cnt_st] = 0x00;
                dot = dot - strlen(st);
                break;
            }
        }
        
        char* str;
        strncpy(str, st, cnt_st - 1)

        byte ip_num = atoi(str);
        byte mask_l = (ip_num >> mod) << mod;                        // mask to the closest dot on the left.
        byte start = mask_l;
        byte end = mask_l | (255 >> (8 - mod));

        // getStartIP(ip, mask);
        cnt = n;
        str = new char[16];
        strncpy(str, ip, dot);
        
        for (int i1 = start; i1 <= end; i1++){
            strcat();
        }
}
*/

// Public interface
//
void IPFilter::load(const char* filename) {
    
    FILE* fp;
    char line[30];

    fp = fopen(filename, "r");

    while (fgets(line, 30, fp)) {
        if (NULL == line)
            continue;
        
        line[strlen(line) - 1] = 0x00;
        
        /*
        char* pch1;
        pch = strstr(line, "/");

        if (NULL == pch)
            this->buildHash(line);
        else {
            this->splitIP(line, pch + 1);
        }
        */
        this->buildHash(line);
    }

    fclose(fp);

}

bool IPFilter::process(char* url) {
    char* pch = strstr(url, "ip=");

    if (NULL == pch)
        return true;

    pch = pch + 3;

    return !this->findHash(pch);
}
