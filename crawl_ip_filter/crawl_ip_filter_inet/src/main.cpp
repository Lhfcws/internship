/**
 * @description IPFilter main.
 * @author Lhfcws Wu （宸风）
 * @time 2013-07-17
 */

#include "ipfilter.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

const int LEN = 1600;
char url[LEN];

int main() {

    IPFilter ipFilter;
    ipFilter.load("/home/wuwenjie/crawl_ip_filter/ip_blacklist");
    //ipFilter.load("/home/wuwenjie/crawl_ip_filter/crawl.ip");

    FILE* fp;
    fp = fopen("/home/wuwenjie/crawl_ip_filter/query", "r");
    //fp = fopen("/home/wuwenjie/crawl_ip_filter/fakedata", "r");
    
    double start = clock();
    while (fgets(url, LEN, fp)) {
        if (ipFilter.process(url)) {
            //printf("%s", url);
            ;
        }
    }
    double end = (clock() - start) / CLOCKS_PER_SEC;
    printf("%lf", end);

    fclose(fp);


 //   getchar();

    return 0;
}
