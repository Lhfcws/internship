#include "BMSQueryRewrite.h"

BMS_BEGIN;

// BMSQueryRewrite Implementation
BMSQueryRewrite::BMSQueryRewrite() {
    _qrw = &BLENDER_INTERFACE::QueryRewrite::getInstance();
}

BMSQueryRewrite::~BMSQueryRewrite() {}

ret_t BMSQueryRewrite::init(const char* conf) {
    if (_qrw->parse(conf))
        return r_succeed;
    else
        return r_failed;
}

int BMSQueryRewrite::rewrite(const char* query, const char* group_name_, std::vector<char *> &rpszResult) {
    SCHE::Query qry_obj;
    char* special_group = qry_obj.getParam(query, strlen(query), "special_group");

    const char* group_name = group_name_;
    if(special_group) {
        group_name = special_group;
    }

    if (!group_name)
        return -1;

    return _qrw->rewrite(query, group_name, rpszResult, DEFAULT_RESULT_COUNT);
}

BMS_END;
