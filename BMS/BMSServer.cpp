#include "BMSServer.h"

BMS_BEGIN;

static bool parse_service_config(const char * src,
		char * protocol, uint32_t protocol_size,
		char * server, uint32_t server_size,
		uint16_t & port) {
	uint32_t len;
	int v;
	if(unlikely(!src || !protocol || !server)) {
		return false;
	}
	while(isspace(*src)) src++;
	len = 0;
	while(!isspace(src[len]) && src[len] != '\0') len++;
	if(src[len] == '\0' || len >= protocol_size) return false;
	memcpy(protocol, src, len);
	protocol[len] = '\0';
	src += len;
	while(isspace(*src)) src++;
	len = 0;
	while(!isspace(src[len]) && src[len] != '\0') len++;
	if(len >= server_size) return false;
	memcpy(server, src, len);
	server[len] = '\0';
	src += len;
	while(isspace(*src)) src++;
	if(*src == '\0') {
		v = atoi(server);
		if(v <= 0) return false;
		port = v;
		server[0] = '\0';
	} else {
		v = atoi(src);
		if(v <= 0) return false;
		port = v;
	}
	return true;
}

BMSServer::BMSServer() {
    _w_factory = NULL;
    _worker_num = 1;
    _locator = NULL;
}

BMSServer::~BMSServer() {
    if (_w_factory) {
        delete _w_factory;
    }
}

void BMSServer::setWorkerNum(int num) {
    _worker_num = num;
}

void BMSServer::init() {
    // Init
    _w_factory = BMSWorkerFactory::getInstance();
    ret_t ret = _w_factory->load_config();
    if (r_succeed != ret)
        printf("Blender Config file loaded fail!\n");
    _locator = new ServerLocator();
    BMSQueryQueue::init();

}

void BMSServer::start() {
    this->initCMClient(clustermap::search, SEARCHER_CFG);
    // this->initCMClient(clustermap::search_doc, DETAILER_CFG);
    // this->allocNodesFromCM();   
    // create workers
    for (int i = 0; i < _worker_num; i++) {
        BMSWorker* worker = _w_factory->makeWorker();
        worker->_locator = _locator;
        worker->setCMClient(clustermap::search, _cm_client[clustermap::search]);
        worker->setCMClient(clustermap::search_doc, _cm_client[clustermap::search_doc]);

        pthread_t pid;
        pthread_create(&pid, NULL, runWorker, (void *)worker);
    }
}

void BMSServer::wait() {
    while (true) {}
}

void BMSServer::recvQuery(char* query) {
    (BMSQueryQueue::getInstance())->push(query);
}

ret_t BMSServer::initCMClient(clustermap::enodetype type, const char* cfg_path) {
    // Load config
    config_t* root = parse_config_file(cfg_path);
    config_group_t* cm_grp = find_config_group(root, "clustermap");
    config_group_t* service_grp = find_config_group(root, "services");
    const char* local_cfg = get_config_value(cm_grp, "local_config_path");
    const char* servers = get_config_value(cm_grp, "cm_server");

    // get spec
    const char *val, *key;
    char protocol[24];
    char server[24];
    uint16_t port;
    char spec[64];

    first_config_item(service_grp);
    // We only care the first item.
    next_config_item(service_grp, &key, &val);
    parse_service_config(val, protocol, 64, server, 64, port);
    snprintf(spec, 64, "tcp:%s:%d", (server?server:""), port);

    // Initialization (include initCMClient, regCMClient, subCMClient)
    int nval;
    std::vector<std::string> cm_servers;

    if(servers) {
        val = servers;
        while(true) {
            while(isspace(*val) || *val == ',') val++;
            if(*val== '\0') break;
            key = val;
            while(*key != '\0' && *key != ',') key++;
            nval = key - val;
            while(nval > 0 && isspace(val[nval - 1])) nval--;
            if(nval > 0) {
                cm_servers.push_back(std::string(val, nval));
            }
            if(*key == '\0') break;
            val = key + 1;
        }
    }
    if(cm_servers.size() ==  0 && !local_cfg) {
        printf("no invalid clustermap configure.\n");
        return r_failed;
    }
    if(_cm_client[type]) delete _cm_client[type];
    _cm_client[type] = new (std::nothrow) clustermap::CMClient();
    if(unlikely(!_cm_client[type])) {
        printf("out of memory at %s:%d.\n", __FILE__, __LINE__ - 2);
        return r_enomem;
    }
    if(_cm_client[type]->Initialize(spec,
                cm_servers, local_cfg) != 0) {
        printf("configure clustermap client (\"%s\", %d, \"%s\") failed.\n",
                SAFE_STRING(spec),
                SAFE_STRING(servers),
                SAFE_STRING(local_cfg));
        delete _cm_client[type];
        _cm_client[type] = NULL;
        return r_failed;
    }
    printf("Initialize clulstermap client succeed.\n");

    // Register
    if(_cm_client[type]->Register() != 0) {
        return r_failed;
    }
    /* What's WatchPoint doing?
    if(_counter) {
        ServerWatchPoint * wp = new (std::nothrow) ServerWatchPoint(*_counter);
        if(unlikely(!wp)) {
            printf("out of memory at %s:%d.\n", __FILE__, __LINE__ - 2);
            return r_enomem;
        }
        if(_cm_client[type]->addWatchPoint(wp)
                != 0) {
            printf("add WatchPoint to clustermap failed.\n");
            delete wp;
        }
    }
    */
    // Subscribe
    if(_cm_client[type]->Subscriber(0) != 0) {
        return r_failed;
    }

    return r_succeed;
}

// Handler for pthread_create
void* runWorker(void* args) {
    BMSWorker* worker = (BMSWorker *)args;
    worker->run();
}

BMS_END;
