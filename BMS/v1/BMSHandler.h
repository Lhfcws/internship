#ifndef BMSHANDLER_H
#define BMSHANDLER_H

#include "BMS.h"
#include "communication.h"
#include "BMSQueryRewrite.h"
#include "dlmodule/DlModuleManager.h"

BMS_BEGIN;

#define IS_UNIQ_POS(p) ((p) < 0)
#define TO_UNIQ_POS(p, j) (-(p + ((j)<<24)))
#define FROM_UNIQ_POS(up, p, j) p = (-(up))&0xFFFFFF; j = ((-(up))&0xFF000000) >> 24;/*fprintf(stderr, "%d\n", (up));*/
static const char LIGHTKEYDELIM = ',';
static const char* DEFAULT_OUTPUT_FORMAT = "xml";

struct BMS_param_t {
    MemPool* heap;
    query::CQueryParameter* param;
    query::CSearchResult* pSearchResult;
};

/**
 * Refer to BlenderWorker
 **/
class BMSHandler {
    public:
        BMSHandler();
        ~BMSHandler();

        ret_t init();
        ret_t handleNewRequest(char* query);
        ret_t handlePhase1Return();
        ret_t handlePhase2Return();

        ret_t deserialPhase1Return(MemPool* heap, query::SClusterData ** & datas, uint32_t &nDocsSearch, uint32_t &nDocsFound, uint32_t &nDocsRestrict);
        ret_t assignDetailerNSend(Communication* comm, char* buf, uint32_t len);
        ret_t processPhase2Result(query::CSearchResult & sr, 
            bool isFinal, MemPool * heap, const char *dfl_cmpr);
        ret_t genResponseData(
            query::CSearchResult * pSearchResult,
            query::CQueryParameter * param,
            char * & data, uint32_t & size, const char *dfl);

        void response(char* res);

        ServerLocator _locator;
    private:
        SCHE::ConnectionPool* _conn_pool;
        anet::Transport _transport;
        config_t* _root;
        int _start, _num;
        SCHE::Query _qry;
        BMSQueryRewrite _qrw;
        std::vector<anet::DefaultPacket> _pvec;
        SORT::BlenderSort _sort;
        APP_DETAIL::ResultFormatFactory _rf_factory;
        APP_BLENDER::ISearchResultProcessor _res_proc;

        BMS_param_t* _args;
        int32_t** pos;
};

BMS_END;

#endif
