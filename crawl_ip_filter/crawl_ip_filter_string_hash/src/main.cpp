/**
 * @description IPFilter main.
 * @author Lhfcws Wu （宸风）
 * @time 2013-07-17
 */

#include "ipfilter.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

const int LEN = 1600;
char url[LEN];

int main() {
    double start = clock();

    IPFilter ipFilter;
    ipFilter.load("/home/wuwenjie/crawl_ip_filter/ip_blacklist1");

    FILE* fp;
    // ifstream fin("/home/wuwenjie/crawl_ip_filter/query");
    //ifstream fin("../test");
    fp = fopen("/home/wuwenjie/crawl_ip_filter/query", "r");
    // string url;

    double process_start = clock();    
    printf("Process begin###########\n");
    while (fgets(url, LEN, fp)) {
        if (ipFilter.process(url)) {
            printf("Yes\n");        
        }
        else
            printf("No\n");
    }

    
    // fin.close();
    fclose(fp);

    double end = clock();
    double during2 = double(end - process_start) / CLOCKS_PER_SEC;
    double during1 = double(end - start) / CLOCKS_PER_SEC;

    printf("Total Time cost:  %lf\n", during1);
    printf("Process Time cost:  %lf\n", during2);

    //getchar();

    return 0;
}
