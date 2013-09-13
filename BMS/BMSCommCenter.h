#ifndef BMSCOMMCENTER_H
#define BMSCOMMCENTER_H

#include "BMS.h"

BMS_BEGIN;

const uint32_t conn_queue_limit = 500;
const uint32_t conn_queue_timeout = 400;


/**
 * Store location information of each role.
 */
class ServerLocator {
    public:
        ServerLocator();
        ~ServerLocator();
        PLIST* setCMNode(const InputProtocol& iprctl_);
        PLIST* setCMNode(clustermap::enodetype type, clustermap::CMISNode* node, bool isTemp);
        PLIST* getNodes(clustermap::enodetype type);
        void freeTempNodes(clustermap::enodetype type);
    private:
        PLIST* _node[clustermap::self + 1];

};

/**
 * Network communication wrapper.
 */
class BMSCommCenter {
    private:
        BMSCommCenter();
        static BMSCommCenter* _center;
        SCHE::ConnectionPool* _conn_pool;
        anet::Transport *_transport;

    public:
        ~BMSCommCenter();
        anet::Connection* makeConnection(clustermap::CMISNode* node_);
        anet::DefaultPacket* makePacket(clustermap::CMISNode* node_, char* query_);

        static BMSCommCenter* getInstance(); 
        static void destroy();

        static void init();
        ret_t send(clustermap::CMISNode* node, anet::DefaultPacket* packet, anet::DefaultPacket* &return_packet);
};

BMS_END;

#endif
