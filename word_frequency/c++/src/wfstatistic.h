/**
 * @description Read English text file and calculate word frequency. 
 * @author Lhfcws Wu (宸风)
 * @time 2013-07-12
 */

#ifndef WFSTATISTIC_H
#define WFSTATISTIC_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <set>
#include <map>

using namespace std;

struct Pair {
    string first;
    int second;
    Pair(string _key, int _value) {
        first = _key;
        second = _value;
    }
};

class WFStatistic {
public:
    WFStatistic(string fname);
    /**
     * @description Output
     */
    void outputSortByKey();
    void outputSortByValue();

    static bool cmpByKey(Pair p1, Pair p2);
    static bool cmpByValue(Pair p1, Pair p2);

    map<string, int> freq;  // map
private:
    vector<string> readFile(); 
    /**
     * @description Scan word and count it.
     */
    void count();

    string filename;
    vector< Pair > pairs;    
    set<char> seps;         // Separators
};

#endif

