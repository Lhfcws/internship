#include <stdio.h>
#include "BMSCommCenter.h"

BMS_BEGIN;


// Implementation of ServerLocator
ServerLocator::ServerLocator(){
    memset(_node, 0x00, sizeof(_node));
}

ServerLocator::~ServerLocator(){
    int i = 0;
    for (; i <= clustermap::self; i++)
        if (_node[i]) {
            PLIST* tempnode = _node[i];
            _node[i] = _node[i]->next;
            delete tempnode;
        }
}

PLIST* ServerLocator::setCMNode(const InputProtocol& iprtcl_) {
    int i = iprtcl_.type;

    PLIST* tempnode = _node[i];


    _node[i] = new PLIST();
    _node[i]->next = tempnode;

    _node[i]->node->m_type = iprtcl_.type;
    strcpy(_node[i]->node->m_ip, iprtcl_.ip);
    _node[i]->node->m_ip[strlen(iprtcl_.ip)] = 0x00;
    _node[i]->node->m_port = iprtcl_.port;
    _node[i]->node->m_protocol = iprtcl_.protocol;

    if (tempnode)
        _node[i]->count = tempnode->count + 1;
    else
        _node[i]->count = 1;
    // m_clustername
    _node[i]->node->m_clustername = strdup(DEFAULT_CLUSTERNAME);
    return _node[i];
}

PLIST* ServerLocator::getNodes(clustermap::enodetype role_) {
    return _node[role_];
}

// Implementation of BMSCommCenter

BMSCommCenter* BMSCommCenter::_center = NULL;

BMSCommCenter::BMSCommCenter () {
    _transport = new anet::Transport();
    _transport->start();

    _conn_pool = new SCHE::ConnectionPool(*_transport, conn_queue_limit, conn_queue_timeout);   
}

BMSCommCenter::~BMSCommCenter () {
    if (_transport) {
        _transport->stop();
        _transport->wait();
        delete _transport;
        _transport = NULL;
    }
    if (_conn_pool) {
        delete _conn_pool;
    }
}

    BMSCommCenter* BMSCommCenter::getInstance() {
        if (!_center)
            _center = new BMSCommCenter();
        return _center;
    }

void BMSCommCenter::destroy() {
    delete _center;
    _center = NULL;
}

anet::Connection* BMSCommCenter::makeConnection (clustermap::CMISNode* node_) {
    char key[64];

    if(node_->m_protocol == clustermap::http) {
        snprintf(key, 64, "tcp:%s:%u", node_->m_ip, node_->m_port);
        return _conn_pool->makeHttpConnection(key);
    } else if(node_->m_protocol == clustermap::tcp) {
        snprintf(key, 64, "tcp:%s:%u", node_->m_ip, node_->m_port);
        return _conn_pool->makeTcpConnection(key);
    } else if(node_->m_protocol == clustermap::udp) {
        TERR("upd protocol is not supnode_->m_portted now.");
        return NULL;
    }
    TERR("unknown protocol.");
    return NULL;
}

anet::DefaultPacket* BMSCommCenter::makePacket (clustermap::CMISNode* node_, char* query_) {
    // Build packet
    anet::DefaultPacket* packet = NULL;

    if(unlikely(!node_)) return NULL;
    if(node_->m_protocol == clustermap::http) {
        anet::HTTPPacket * p = new (std::nothrow) anet::HTTPPacket;
        if(unlikely(!p)) return NULL;
        p->setMethod(anet::HTTPPacket::HM_GET);
        if(query_ && strlen(query_) > 0) p->setURI(query_);
        else p->setURI("");
        p->addHeader("Accept", "*/*");
        p->addHeader("Connection", "Keep-Alive");
        packet = p;
    } else if(node_->m_protocol == clustermap::tcp) {
        anet::DefaultPacket * p = new (std::nothrow) anet::DefaultPacket;
        if(unlikely(!p)) return NULL;
        if(query_ && strlen(query_) > 0) p->setBody(query_, strlen(query_));
        else p->setBody("", 0);
        packet = p;
    } else if(node_->m_protocol == clustermap::udp) {
        //TODO: create udp packet
    }

    return packet;
}

void BMSCommCenter::init() {
    _center = NULL;
}

ret_t BMSCommCenter::send(clustermap::CMISNode* node, anet::DefaultPacket* packet, anet::DefaultPacket* &return_packet) {

    anet::Connection* conn = this->makeConnection(node);

    return_packet = NULL;
    return_packet = dynamic_cast<anet::DefaultPacket*>(conn->sendPacket(packet));
    printf("Get return packet. \n");
    if (!return_packet)
        return r_failed;
    return r_succeed;
}
BMS_END;
