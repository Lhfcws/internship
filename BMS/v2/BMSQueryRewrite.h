#ifndef BMSQUERYREWRITE_H
#define BMSQUERYREWRITE_H

#include "BMS.h"

#include "sche/application/blender/QueryRewrite.h"


BMS_BEGIN;

class BMSQueryRewrite{
private:
    BLENDER_INTERFACE::QueryRewrite* _qrw;
public:
    BMSQueryRewrite();
    ~BMSQueryRewrite();
    ret_t init(const char* conf);
    int rewrite(const char *query, const char* group_name, std::vector<char*> &rpszResult);
};

BMS_END;

#endif
