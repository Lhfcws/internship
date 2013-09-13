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

BMSWorker* BMSWorkerFactory::makeWorker() {
    _count++;
    return new BMSWorker();
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

BMS_END;
