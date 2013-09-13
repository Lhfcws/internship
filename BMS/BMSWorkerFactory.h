#ifndef BMSWORKERFACTORY_HPP
#define BMSWORKERFACTORY_HPP

#include "BMS.h"
#include "BMSWorker.h"


BMS_BEGIN;

// Singleton
class BMSWorkerFactory {
    private:
        BMSWorkerFactory(){}
        static BMSWorkerFactory* _factory;
        static int _count;

        config_t* _root;
        BMSQueryRewrite _qrw;
        SORT::BlenderSort _sort;
        APP_DETAIL::ResultFormatFactory _rf_factory;
    public:
        ~BMSWorkerFactory();
        static void init();

        static BMSWorkerFactory* getInstance();

        int getWorkerNum() const;

        BMSWorker* makeCleanWorker();
        BMSWorker* makeWorker();
        ret_t load_config();

        void destroyWorker(BMSWorker* &worker);

        static void destroy();
};


BMS_END;
#endif
