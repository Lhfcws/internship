#ifndef COMMUNICATION_H
#define COMMUNICATION_H

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
        PLIST* getNodes(clustermap::enodetype role_);
    private:
        PLIST* _node[clustermap::self + 1];

};

/**
 * Network communication wrapper.
 */
class Communication {
    private:
        clustermap::CMISNode* _node;

    public:
        Communication(clustermap::CMISNode* node);
        Communication();
        ~Communication();

        anet::Connection* _conn;

        /**
         * Make a connection
         */
        anet::Connection* makeConnection(SCHE::ConnectionPool* conn_pool); 
        /**
         * set CMISNode to communicate
         */
        void setNode(clustermap::CMISNode* node_);
        /**
         * Make a packet common interface
         */
        anet::DefaultPacket* makePacket(char* query_);
        /**
         * Send packet common interface
         */
        ret_t sendPacket(anet::DefaultPacket* &return_packet, anet::DefaultPacket* packet, anet::Connection* conn);
        /**
         * Tcp Send wrapper
         */
        anet::Packet* tcpSendPacket(anet::TCPConnection* tcp_conn, anet::DefaultPacket* packet);
        /**
         * call Socket.read & pack data into anet::DefaultPacket
         */
        int readPacketFromSocket(anet::Socket* socket, anet::DefaultPacket* &packet);
};

BMS_END;

#endif
