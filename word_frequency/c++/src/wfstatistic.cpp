/**
 * @description Implementation of `wfstatistic.h`. 
 * @author Lhfcws Wu (宸风)
 * @time 2013-07-12
 */

#include <algorithm>
#include <sstream>
#include <cctype>
#include "wfstatistic.h"

/**
 * @description Judge a string if it is a number string. Compatible with string or char.
 */
template <class S>
bool isNumber(S str) {
    double d;
    S sr;

    stringstream ss, ss1;
    // string to double
    ss << str;
    ss >> d;
    // double to string
    ss1 << d;
    ss1 >> sr;

    return str == sr;
}


bool WFStatistic::cmpByValue(Pair p1, Pair p2) {
    return (p1.second < p2.second);
}

bool WFStatistic::cmpByKey(Pair p1, Pair p2) {
    return (p1.first < p2.first);
}

/**
 * WFStatistic implementation.
 */
WFStatistic::WFStatistic(string filename) {
    this->filename = filename;
    this->freq.clear();
    char __seps[] = {' ', ',', '.', ':', '\'', '\"', '?', '!', '(', ')', '/', '&'};
    set<char> __set(__seps, __seps+12);
    this->seps = __set;

    this->count();
}

void WFStatistic::outputSortByKey() {
    sort(this->pairs.begin(), this->pairs.end(), WFStatistic::cmpByKey);
    cout << this->filename << endl;
    for (size_t i=0;i<this->pairs.size();i++) {
        cout << (this->pairs[i]).first << ": " << (this->pairs[i]).second << endl;
    }
    cout << endl;
}

void WFStatistic::outputSortByValue() {
    sort(this->pairs.begin(), this->pairs.end(), WFStatistic::cmpByValue);
    cout << this->filename << endl;
    for (size_t i=0;i<this->pairs.size();i++) {
        cout << (this->pairs[i]).first << ": " << (this->pairs[i]).second << endl;
    }
}

vector<string> WFStatistic::readFile() {
    string str;
    vector<string> vec;
    vec.clear();

    ifstream fin(this->filename.c_str());

    while (getline(fin, str)) {
        vec.push_back(str);
       
    }

    fin.close();

    return vec;
}

void WFStatistic::count() {
    vector<string> text = this->readFile();
    string prev;
    string new_word;
    char ch;

    for (size_t line=0; line<text.size(); line++) {        // Scan by each file line.
        for (size_t i=0; i<text[line].length(); i++) {     // Scan by each char
            ch = tolower(text[line][i]);
            if (prev.length() > 0 && ch == '.' && isNumber(prev) && i != text[line].length() - 1 && isNumber(prev[i+1]))     // Condition: 'I am 12.3 feet.'
                continue;
            if (seps.find(ch) != seps.end() && prev.length() > 0) {     // Condition: ch is a separator.
                new_word = prev;
                prev = "";
                if (freq.find(new_word) != freq.end()) {
                    freq.find(new_word)->second += 1;
                }
                else
                    freq.insert(make_pair(new_word, 1));
            }

            if (seps.find(ch) == seps.end())        // Condition: ch is a normal char, such as a letter.
                prev = prev + ch;
        }
    }

    map<string, int>::iterator iter = this->freq.begin();
    
    for (; iter != this->freq.end(); iter++) {
        this->pairs.push_back(Pair(iter->first, iter->second));
    }
}
