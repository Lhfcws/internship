#include "CLParser.h"

CLP_BEGIN;

void CLParser::add_parameter(std::string short_opt, std::string long_opt, fun_t handler, int para_num) {
    if (!handler || para_num < 0)
        return;

    Option option;
    option.short_opt = short_opt;
    option.long_opt = long_opt;
    option.para_num = para_num;
    option.fn = handler;

    _optlist.push_back(option);
}

void CLParser::parse(int argc, char* argv[]) {
    int i = 1;
    Option opt_help;
    find(_optlist, "-h", opt_help);

    while (i < argc) {
        if (i == 0 && argv[i][0] != '-') {
            i++;
            continue;
        }
        if (argv[i][0] != '-') {
            opt_help.fn("OMG");
        }

        std::string opt = argv[i];
        Option option;

        bool bo = find(_optlist, opt, option);

        if (!bo) {
            opt_help.fn("ta si ge te");
        }

        // For now we assume that there're at most 1 argument.
        i++;
        if (i >= argc)
            opt_help.fn("ta si ge te");

        option.fn(std::string(argv[i]));

        i++;
    }
}
CLP_END;
