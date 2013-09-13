#ifndef CLPARSER_H
#define CLPARSER_H

#include <string>
#include <map>
#include <vector>

#define CLP_BEGIN namespace CLP {
#define CLP_END }

CLP_BEGIN;

typedef void (*fun_t)(const std::string&);

struct Option {
    std::string short_opt;
    std::string long_opt;
    int para_num;
    fun_t fn;
};

inline bool find(const std::vector<Option>& optlist, const std::string& opt, Option& result) {
    for (int i = 0; i < optlist.size(); i++) {
        if (optlist[i].short_opt == opt || optlist[i].long_opt == opt) {
            result = optlist[i];
            return true;
        }
    }
    return false;
}

class CLParser {
private:
    std::vector<Option> _optlist;
    std::map<std::string, int> _args;
public:
    CLParser() {
        _optlist.clear();
        _args.clear();
    }
    ~CLParser() {
        _optlist.clear();
        _args.clear();
    }

    void add_parameter(std::string short_opt, std::string long_opt, fun_t handler, int para_num = 0);
    void parse(int argc, char* argv[]);
};

CLP_END;
#endif
