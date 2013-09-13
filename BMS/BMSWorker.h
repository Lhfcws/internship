#ifndef BMSWORKER_H
#define BMSWORKER_H

#include "BMS.h"
#include "BMSCommCenter.h"
#include "BMSQueryRewrite.h"
#include "BMSQueryQueue.h"
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
class BMSWorker {
    public:
        BMSWorker(
                config_t* root,
                SORT::BlenderSort sort_,
                BMSQueryRewrite qrw_,
                APP_DETAIL::ResultFormatFactory rf_factory);
        BMSWorker();
        ~BMSWorker();

        void run();

        ret_t server_init();
        ret_t handleNewRequest(char* query);
        ret_t handlePhase1Return();
        ret_t handlePhase2Return();

        ret_t deserialPhase1Return(MemPool* heap, query::SClusterData ** & datas, uint32_t &nDocsSearch, uint32_t &nDocsFound, uint32_t &nDocsRestrict);
        ret_t processPhase2Result(query::CSearchResult & sr, 
                bool isFinal, MemPool * heap, const char *dfl_cmpr);
        ret_t genResponseData(
                query::CSearchResult * pSearchResult,
                query::CQueryParameter * param,
                char * & data, uint32_t & size, const char *dfl);

        void response(char* res);

        void setCMClient(clustermap::enodetype type, clustermap::CMClient* cm_client);
        void tempAllocNodesFromCM(clustermap::enodetype type);
        void freeTempAllocedNodes(clustermap::enodetype type);

        ServerLocator* _locator;
    private:
        std::vector<anet::DefaultPacket> _pvec;
        int _start, _num;
        SCHE::Query _qry;
        APP_BLENDER::ISearchResultProcessor _res_proc;
        clustermap::CMClient*_cm_client[clustermap::self + 1];

        config_t* _root;
        BMSQueryRewrite _qrw;
        SORT::BlenderSort _sort;
        APP_DETAIL::ResultFormatFactory _rf_factory;

        BMS_param_t* _args;
};

BMS_END;

#endif
