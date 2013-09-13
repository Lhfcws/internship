#include "BMSWorker.h"

BMS_BEGIN;

static inline uint32_t _write_wrap(query::CSearchResult & sr,
        query::CQueryParameter & param,
        char * data, uint32_t size) {

    uint32_t cost, len;
    cost = 0;
    len = snprintf(data, size, "_et_=%d&dl=%s",
            param.nEtTime, param.m_szDlContent);
    if(len > size) return size + 1;
    cost += len;

    if(sr.m_pLightKeys) {
        if(cost + 12 > size) return size + 1;
        memcpy(data + cost, "&_lightkey_=", 12);
        cost += 12;
        for(int i = 0; i < sr.m_pLightKeys->size(); i++) {
            char * szKeys = sr.m_pLightKeys->m_szArray[i];
            if(!szKeys) continue;
            len = strlen(szKeys);
            if(len == 0) continue;
            if(len * 4 + 1 + cost > size) return size + 1;
            string_utility::urlEncode2(data + cost, szKeys);
            cost += strlen(data + cost);
            *(data+cost) = '\1';
            cost++;
        }
    }
    return cost;
}


void releaseVector(std::vector<char *>& vec) {
    if (vec.empty()) return;
    for (int32_t i = 0; i < vec.size(); i++) {
        if(vec[i]) {
            delete[] vec[i];
            vec[i] = NULL;
        }
    }
    vec.clear();
}

// BMSHanlder Implementation

BMSWorker::BMSWorker(
        config_t* root_,
        SORT::BlenderSort sort_,
        BMSQueryRewrite qrw_,
        APP_DETAIL::ResultFormatFactory rf_factory_):

    _root(root_), _sort(sort_), _qrw(qrw_),
    _rf_factory(rf_factory_) 
{
    memset(_cm_client, 0x00, sizeof(_cm_client));
}


BMSWorker::BMSWorker() {
    _root = NULL;
    memset(_cm_client, 0x00, sizeof(_cm_client));
}

BMSWorker::~BMSWorker() {
    if (_root) {
        ::free(_root);
    }

    if (_args) {
        delete _args;
    }
}

ret_t BMSWorker::server_init() {
    ret_t ret;

    // Load config
    // const char* blender_cfg = "/home/lhfcws/ISEARCH/etc/isearch/blender_server.cfg";
    _root = parse_config_file(BLENDER_CFG);

    config_group_t * grp = get_root_config_group(_root);

    const char* blender_dl_cfg = get_config_value(grp, "module_conf_path");

    const char* sort_config = get_config_value(grp, "sort_config");
    ret = _sort.initialize(sort_config, sort_config);
    if (ret != r_succeed)
        return ret;

    const char* result_format_cfg = get_config_value(grp, "result_format_conf_file");
    int iret = _rf_factory.init(result_format_cfg);

    dlmodule_manager::CDlModuleManager::getModuleManager()->parse(blender_dl_cfg);
    ret = _qrw.init(blender_dl_cfg);
    if (unlikely(iret != 0 || ret != r_succeed))
        return r_failed;

    return r_succeed;
}

// NewRequest 

ret_t BMSWorker::handleNewRequest(char* query) {

    printf("##New Request Send!!\n");
    if (unlikely(!query))
        return r_failed;

    ret_t ret;

    _qry.setOrigQuery(query, strlen(query));
    _qry.purge(QUERY_PREFIX);

    if (unlikely(_qry.getPureQuerySize() <= 0))
        return r_failed;

    const char* pure_query = _qry.getPureQueryData();

    // Query Rewrite (refer to ISearchResultProcessor:allocPhase1Request)
    int iRes = -1;
    std::vector<char*> qvec;
    qvec.clear();

    iRes = _qrw.rewrite(pure_query, DEFAULT_CLUSTERNAME, qvec);
    if (iRes == -1) {
        releaseVector(qvec);
        return r_failed;
    }
    if (qvec.empty())
        return r_failed;

    _qry.setIterator(iRes);
    ret = _qry.setRewriteQueryGroup(qvec);

    if (ret != r_succeed) {
        releaseVector(qvec);
        return r_failed;
    }

    char* first = strdup(qvec[0]);
    uint32_t first_size = strlen(first);
    _qry.setRewriteQuery(first, first_size);

    ret = _res_proc.getIntValue(first, first_size, "s", _start);
    if((ret!=r_succeed) && (ret!=r_uninitialized)) {
        ::free(first);
        releaseVector(qvec);
        return ret;
    }

    ret = _res_proc.getIntValue(first, first_size, "n", _num);
    if (ret == r_uninitialized)
        _num = DEFAULT_RESULT_COUNT;
    else if (ret != r_succeed) {
        ::free(first);
        releaseVector(qvec);
        return ret;
    }

    char* p1_data = NULL;
    uint32_t p1_size = 0;

    ret = _res_proc.genPhase1Query(first, first_size,
            _start, _num,
            p1_data, p1_size);

    ::free(first);

    // send packet
    _pvec.clear();
    PLIST* piter = _locator->getNodes(clustermap::search);
    if (piter == NULL)
        this->tempAllocNodesFromCM(clustermap::search);

    for (; piter; piter = piter->next) {
        anet::DefaultPacket* packet = NULL;
        anet::DefaultPacket* snd_packet = BMSCommCenter::getInstance()->makePacket(piter->node, p1_data);

        ret = BMSCommCenter::getInstance()->send(piter->node, snd_packet, packet);

        if (ret != r_succeed)
            return ret;

        if (packet->isRegularPacket()) {
            // Process return packet slice
            _pvec.push_back(*packet);
        }

    }

    this->freeTempAllocedNodes(clustermap::search);

    return r_succeed;
}


// Phase1 return handler
ret_t BMSWorker::handlePhase1Return() {

    printf("##Phase 1 Request Send!!\n");

    ret_t ret;
    APP_BLENDER::MemPoolFactory mp_factory;
    MemPool* heap = mp_factory.make();
    uint32_t nDocsSearch, nDocsFound, nDocsRestrict;
    query::SClusterData ** datas;
    query::CQueryParameter * param;
    query::CSearchResult * pSearchResult;
    query::SortData sortdata;

    ret = deserialPhase1Return(heap, datas, nDocsSearch, nDocsFound, nDocsRestrict);
    if (unlikely(ret != r_succeed)) {
        return ret;
    }
    pSearchResult = NEW(heap, query::CSearchResult)(NULL, heap);

    // Construct last_result, it may be useless now.
    app_blender::Phase1TempResult * last_rst = (app_blender::Phase1TempResult*)NEW(heap, app_blender::Phase1TempResult);
    if (unlikely(!last_rst)) {
        return r_enomem;
    }

    last_rst->datas = NULL;
    last_rst->count = 0;
    last_rst->req = NULL;
    last_rst->result = NULL;
    last_rst->nDocsSearch = nDocsSearch;
    last_rst->nDocsFound = nDocsFound;
    last_rst->nDocsRestrict = nDocsRestrict;
    last_rst->datas = datas;
    last_rst->count = _pvec.size();

    // Construct param
    uint32_t n;
    for(n = last_rst->count; n > 0; n--) {
        if (last_rst->datas[n - 1]) {
            param = last_rst->datas[n - 1]->m_pQueryParameter;
            if(param) break;
        }
    }

    if (!param)
        return r_failed;

    param->nStart = _start;
    param->nNum = _num;

    // Construct pSearchResult
    pSearchResult->m_nDocsSearch = nDocsSearch;
    pSearchResult->m_pQueryParameter = param;
    pSearchResult->m_nDocsFound  = nDocsFound;
    pSearchResult->m_nDocsRestrict = nDocsRestrict;

    // Construct sortData && sort process
    if (_qry.getRewriteQueryData() && strstr(_qry.getRewriteQueryData(), "app=p4p")) {
        sortdata.query = _qry.getRewriteQueryData();
    } else {
        sortdata.query = _qry.getPureQueryData();
    }

    sortdata.iSubStart = _start;
    sortdata.iSubNum = _num;
    ret = _sort.process(&sortdata,
            last_rst->datas, last_rst->count,
            param, pSearchResult,
            heap);

    if (unlikely(ret != r_succeed))
        return ret;

    pSearchResult->m_eKeyExp = param->m_eKeyExp;
    pSearchResult->m_pEchoKeys = param->m_pEchoKeys;
    pSearchResult->m_pOwDetail = param->m_pOwDetail;
    pSearchResult->m_pLightKeys = param->m_pLightKeys;
    pSearchResult->m_szDictBuffer = param->m_szDictBuffer;
    pSearchResult->m_nDictBufLen = param->m_nDictBufferLen;

    last_rst->result = pSearchResult;

    // _et_ , dl, _lightkey_
    char* wrap_data;
    uint32_t wrap_size = 256, cost;
    while(true) {
        wrap_data = NEW_VEC(heap, char, wrap_size);
        if(unlikely(!wrap_data)) return r_enomem;

        cost = _write_wrap(*pSearchResult, *param, wrap_data, wrap_size);
        if(cost <= wrap_size) {
            wrap_size = cost;
            break;
        }
        wrap_size = wrap_size * 2;
    }

    if (0 >= pSearchResult->m_nDocsReturn) {
        return r_failed;
    }

    uint32_t i = 0;
    char* buf;

    // gen phase2 request string
    _pvec.clear();
    ret = _res_proc.genSimplePhase2RequestString(wrap_data, wrap_size,
            *pSearchResult, heap, buf, cost);

    printf("\n#Phase1 Send packet!!\n");
    printf("%s\n", buf);

    PLIST* piter = _locator->getNodes(clustermap::search_doc);
    if (piter == NULL)
        this->tempAllocNodesFromCM(clustermap::search_doc);

    for (; piter != NULL; piter++) {

        anet::DefaultPacket* packet = NULL;
        anet::DefaultPacket* snd_packet = BMSCommCenter::getInstance()->makePacket(piter->node, buf);

        ret = BMSCommCenter::getInstance()->send(piter->node, snd_packet, packet);
        if (unlikely(ret != r_succeed))
            return ret;
        if (packet->isRegularPacket()) {
            // Process return packet slice
            _pvec.push_back(*packet);
        }
    }

    this->freeTempAllocedNodes(clustermap::search_doc);

    _args = new BMS_param_t;
    _args->heap = heap;
    _args->pSearchResult = pSearchResult;
    _args->param = param;

    return r_succeed;
}

// Phase2 return handler
ret_t BMSWorker::handlePhase2Return() {

    printf("##Phase 2 Request Send!!\n");

    query::CQueryParameter * pParam;
    query::CSearchResult * pSearchResult;
    MemPool * pHeap;
    BMS_param_t *pBMS_param;
    query::SearchResultSerializer serial;
    ret_t ret;
    char * pRes_data;
    uint32_t unRes_size, cnt;

    pBMS_param = (BMS_param_t *)_args;
    if (unlikely(!pBMS_param)) {
        printf("getArgs()失败");
        return r_failed;
    }
    pHeap = pBMS_param->heap;
    pParam = pBMS_param->param;
    pSearchResult = pBMS_param->pSearchResult;
    if (unlikely(!pHeap)) {
        printf("缺少MemPool\n");
        return r_failed;
    }
    if (unlikely(!pBMS_param) || unlikely(!pSearchResult)) {
        printf("getArgs()失败\n");
        return r_failed;
    }

    ret = this->processPhase2Result(*pSearchResult, true, pHeap, NULL);

    if (ret != r_succeed) {
        printf(" processPhase2Result()失败\n");
        return r_failed;
    }

    if (!pSearchResult->LoadVsaToDocument()) {
        printf(" LoadVsaToDocument()失败\n");
        return r_failed;
    }

    cnt = pSearchResult->m_nDocsReturn;
    if ((BLENDER_INTERFACE::DocumentRewrite::getInstance().size() > 0) && (cnt > 0)) {
        BLENDER_INTERFACE::DRWParam param;
        param.query_string = _qry.getPureQueryData();
        param.query_size = _qry.getPureQuerySize();
        param.rewrite_group_query = _qry.getRewriteQueryGroup();
        param.rewrite_group_size = _qry.getQueryTimes();
        param.m_iSubStart = _start;
        param.m_iSubNum = _num;
        param.m_piServerId = pSearchResult->m_nServerID;
        param.col_num = 1;//列数
        if (!param.mergeParams()) {
            return r_failed;
        }
        int32_t lightkey_size = 0;
        int32_t cost_size = 0;
        basis::n32_t len = 0;

        if (pSearchResult->m_pLightKeys) {
            for (int32_t i=0; i<pSearchResult->m_pLightKeys->size(); i++) {
                lightkey_size += strlen((*(pSearchResult->m_pLightKeys))[i])+1;
            }
        }

        param.match_keywords = NULL;
        param.matchkeyword_size = 0;
        if (lightkey_size > 0) {
            char *pKeystr = (char *)NEW_VEC(pHeap, char, sizeof(char)*lightkey_size+1);
            if (!pKeystr) {
                return r_enomem;
            }

            for (int32_t i = 0; i<pSearchResult->m_pLightKeys->size(); i++) {
                cost_size += snprintf(pKeystr+cost_size, lightkey_size-cost_size, "%s%c",
                        (*(pSearchResult->m_pLightKeys))[i], LIGHTKEYDELIM);
            }
            pKeystr[cost_size] = '\0';

            param.match_keywords = pKeystr;
            param.matchkeyword_size = cost_size;
        }
        if (BLENDER_INTERFACE::DocumentRewrite::getInstance().rewrite(pSearchResult->m_ppDocuments, cnt, param) != r_succeed) {
            printf("BLENDER_INTERFACE::DocumentRewrite失败\n");
            return r_failed;
        }
    }
    pSearchResult->m_nDocsReturn = cnt;

    uint32_t j = 0;
    for (uint32_t i = 0; i < pSearchResult->m_nDocsReturn; i++ ) {
        if (pSearchResult->m_ppDocuments[i] == NULL) {
            j++;
        }
    }
    if (j > 0) {
        _qry.setUseCache(false);
    }
    ret = this->genResponseData(pSearchResult, pParam,
            pRes_data, unRes_size, DEFAULT_OUTPUT_FORMAT);
    if (unlikely(ret != r_succeed)) {
        if (pRes_data) {
            ::free(pRes_data);
        }
        printf("genResponseData()失败\n");
        return ret;
    }

    this->response(pRes_data);
    return r_succeed;
}

ret_t BMSWorker::deserialPhase1Return(MemPool* heap, query::SClusterData ** & datas, uint32_t &nDocsSearch, uint32_t &nDocsFound, uint32_t &nDocsRestrict) {
    bool hasSucceed_flag = false;

    PLIST* phead = _locator->getNodes(clustermap::search);
    PLIST* piter = _locator->getNodes(clustermap::search_doc);

    int phase1_req_size = phead->count;

    // refer to ISearchResultProcessor:deserialPhase1Result
    datas = (query::SClusterData**) NEW_VEC(heap, char, sizeof(query::SClusterData*) * _pvec.size());
    nDocsSearch = 0;
    nDocsFound = 0;
    nDocsRestrict = 0;
    query::SearchResultSerializer serial(heap);
    ret_t ret;
    int i = 0;

    for (; i < phase1_req_size; i++, piter = piter->next) {
        datas[i] = (query::SClusterData*) NEW_VEC(heap, char, sizeof(query::SClusterData));
        if(!datas[i]) 
            return r_enomem;

        if(!serial.deserialPhase1(_pvec[i].getBody(), _pvec[i].getBodyLen(),
                    heap, *(datas[i]), NULL)) {
            datas[i] = NULL;
            continue;
        }
        if(!datas[i]->m_pQueryParameter)
        {
            datas[i] = NULL;
            continue;
        }
        datas[i]->m_szClusterName = piter->node->m_clustername;

        nDocsSearch += datas[i]->m_nDocsSearch;
        nDocsFound += datas[i]->m_nDocsFound;
        nDocsRestrict += datas[i]->m_nDocsRestrict;
        datas[i]->nServerId = i;

        hasSucceed_flag = true;
    }

    if (hasSucceed_flag)
        return r_succeed;
    else 
        return r_failed;
}

// refer to ISearchResultProcessor:processPhase2Result
ret_t BMSWorker::processPhase2Result(query::CSearchResult & sr, 
        bool isFinal, MemPool * heap, const char *dfl_cmpr) {

    document::CDocument **pDocs;
    unsigned char *pOnline;
    uint32_t count;
    SCHE::ANETRequest *r;
    SCHE::ANETResponse *res;
    clustermap::CMISNode *node;
    query::SearchResultSerializer serial(heap);

    if (unlikely(!heap))
        return r_failed;

    sr.m_ppDocuments = (document::CDocument**)NEW_VEC(heap, document::CDocument*, sr.m_nDocsReturn);
    if (unlikely(!sr.m_ppDocuments))
        return r_enomem;

    memset(sr.m_ppDocuments, 0, sizeof(document::CDocument*) * sr.m_nDocsReturn);
    sr.m_pOnlineStatus = (unsigned char*)NEW_VEC(heap, unsigned char, sr.m_nDocsReturn);
    if (unlikely(!sr.m_pOnlineStatus))
        return r_enomem;

    PLIST* piter = _locator->getNodes(clustermap::search_doc);
    int phase2_req_size = piter->count;

    for(int i = 0; i < phase2_req_size; i++, piter++) {

        node = piter->node;
        if (unlikely(!node))
            continue;
        if (!serial.deserialPhase2(_pvec[i].getBody(), _pvec[i].getBodyLen(), heap, pDocs, pOnline, count, dfl_cmpr)) {
            continue;
        }
        for (int j = 0; j < count; j++) {
            sr.m_ppDocuments[j] = pDocs[j];
            sr.m_pOnlineStatus[j] = pOnline[j];
            if (isFinal) {
                document::CField *clname_field = NEW(heap, document::CField)("__buildin_src", node->m_clustername, ft_string, sl_English);
                if (unlikely(!clname_field))
                    return r_enomem;
                sr.m_ppDocuments[j]->addField(clname_field, true);
            }
        }
    }

    return r_succeed;
}

// refer to ISearchResultProcessor:genResponseData
ret_t BMSWorker::genResponseData(
        query::CSearchResult * pSearchResult,
        query::CQueryParameter * param,
        char * & data, uint32_t & size, const char *dfl)
{
    char * outfmt;
    APP_DETAIL::result_container_t container;
    APP_DETAIL::ResultFormat * rf;
    ret_t ret;
    if (_qry.getRewriteQuerySize() > 0) { 
        outfmt = SCHE::Query::getParam(_qry.getRewriteQueryData(),
                _qry.getRewriteQuerySize(),
                "outfmt");
    }
    else
    {
        outfmt = SCHE::Query::getParam(_qry.getPureQueryData(),
                _qry.getPureQuerySize(),
                "outfmt");
    }
    rf = _rf_factory.make(
            (outfmt && strlen(outfmt) > 0) ? outfmt : dfl);
    if(outfmt) ::free(outfmt);

    // 如果outfmt 参数错误，导致rf为0，重新用默认dfl构造rf一次
    if (rf == 0)
        rf = _rf_factory.make(dfl);

    if(unlikely(!rf)) return r_failed;

    //container.query = _qry.getPureQueryData();
    //读取经_qrw改写后的query查询串，而不是未经改写的原始查询串.
    if (_qry.getRewriteQuerySize() > 0)
    {
        container.query = _qry.getRewriteQueryData();
    }
    else
    {
        container.query = _qry.getPureQueryData();
    }
    container.search_result = pSearchResult;
    container.param = param;
    container.cost = 0;
    ret = rf->format(container, data, size);
    _rf_factory.recycle(rf);
    return ret;
}

void BMSWorker::run() {

    printf("A new worker running!\n");

    char* query;
    ret_t ret;

    do {
        BMSQueryQueue::getInstance()->pop(query);    
        printf("Worker get a query.\n");

        ret = this->handleNewRequest(query);
        if (r_succeed == ret)
            ret = this->handlePhase1Return();
        if (r_succeed == ret)
            ret = this->handlePhase2Return();
        printf("worker finished a query.\n");
    } while(true);
}

void BMSWorker::response(char* res) {
    /*
       FILE* fp = fopen("result.xml", "w");
       fprintf(fp, "%s\n", res);

       fclose(fp);
       */
    printf("%s\n", res);
}

void BMSWorker::setCMClient(clustermap::enodetype type, clustermap::CMClient* cm_client) {
    _cm_client[type] = cm_client;
}

void BMSWorker::tempAllocNodesFromCM(clustermap::enodetype type) {
    clustermap::CMISNode** nodes;
    int32_t nodes_count;
    ret_t ret = _res_proc.allocNodeFromCM(*_cm_client[type], type, 1, clustermap::EQ, nodes, nodes_count, 0);

    // Extra limit because of the reality
    if (nodes_count > 2 && type == clustermap::search_doc)
        nodes_count = 2;

    // Store temp nodes into locator
    PLIST* pnode = _locator->getNodes(type);
    PLIST* pprev = pnode;
    while (pnode != NULL) {
        pprev = pnode;
        pnode = pnode->next;
    }

    uint32_t i = 0;
    for (; i < nodes_count; i++) {
        _locator->setCMNode(type, nodes[i], true);
    }
}

void BMSWorker::freeTempAllocedNodes(clustermap::enodetype type) {
    _locator->freeTempNodes(type);
}

BMS_END;
