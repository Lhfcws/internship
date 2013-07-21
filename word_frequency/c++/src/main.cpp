/**
 * @description Main of Word frequency statistic program. 
 * @author Lhfcws Wu (宸风)
 * @time 2013-07-13
 */
#include <iostream>
#include <algorithm>
#include "wfstatistic.h"

string path = "../../resource/";
const int MAXFILE = 3;
map<string, int> mp1;
vector< Pair > pairs;    


void getTotalStat(vector<WFStatistic> vec) {
    mp1.clear();
    pairs.clear();
    
    // Merge multi-files freq
    for (size_t i=0; i<vec.size(); i++) {
        map<string, int>::iterator iter = vec[i].freq.begin();
        while (iter != vec[i].freq.end()) {
            if (mp1.find(iter->first) == mp1.end()) {
                mp1.insert(make_pair(iter->first, iter->second));
            }
            else {
                (mp1.find(iter->first))->second += iter->second;
            }
            iter++;
        }
    }

    for (map<string, int>::iterator iter = mp1.begin(); iter != mp1.end(); iter++) {
        pairs.push_back(Pair(iter->first, iter->second));
    }

}

void outputSortByKeySum() {
    cout << endl << "Totally sort by key: " << endl;
    sort(pairs.begin(), pairs.end(), WFStatistic::cmpByKey);

    for (size_t i=0; i<pairs.size(); i++) {
        cout << pairs[i].first << ":  " << pairs[i].second << endl;
    }
    cout << endl;
}

void outputSortByValueSum() {
    cout << endl << "Totally sort by value: " << endl;
    sort(pairs.begin(), pairs.end(), WFStatistic::cmpByValue);

    for (size_t i=0; i<pairs.size(); i++) {
        cout << pairs[i].first << ":  " << pairs[i].second << endl;
    }
    cout << endl;

}

int main(int args, char** argv) {
    string filename;
    vector<WFStatistic> vec;
    vec.clear();
    mp1.clear();

    for (char i='1'; i<=(char)(MAXFILE + 48); i++) {
        filename = path + "test" + i + ".txt";

        WFStatistic wfs(filename);
    
        // Output by each file
        cout << endl;
        cout << filename << " # Key #: " << endl;
        wfs.outputSortByKey();

        cout << endl;
        cout << filename << " # Value #: " << endl;
        wfs.outputSortByValue();

        // Put the new object into a vactor.
        vec.push_back(wfs);
    }

    getTotalStat(vec);
    outputSortByKeySum();
    outputSortByValueSum();


    return 0;
}
