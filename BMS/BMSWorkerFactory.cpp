#include "BMSWorkerFactory.h"

BMS_BEGIN;

BMSWorkerFactory* BMSWorkerFactory::_factory = NULL;
int BMSWorkerFactory::_count = 0;


BMSWorkerFactory::~BMSWorkerFactory(){
    init();
}


BMSWorkerFactory* BMSWorkerFactory::getInstance() {
    if (!_factory) {
        _factory = new BMSWorkerFactory();
    }
    return _factory;
}

void BMSWorkerFactory::init() {
    _factory = NULL;
    _count = 0;

}

int BMSWorkerFactory::getWorkerNum() const {
    return _count;
}

BMSWorker* BMSWorkerFactory::makeCleanWorker() {
    _count++;
    return new BMSWorker();
}

BMSWorker* BMSWorkerFactory::makeWorker() {
    _count++;
    return new BMSWorker(_root, _sort, _qrw, _rf_factory);
}

void BMSWorkerFactory::destroyWorker(BMSWorker* &worker) {
    delete worker;
    _count--;
}

void BMSWorkerFactory::destroy() {
    if (_factory) {
        delete _factory;
    }
    _factory = NULL;
    _count = 0;
}

ret_t BMSWorkerFactory::load_config() {
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
BMS_END;
