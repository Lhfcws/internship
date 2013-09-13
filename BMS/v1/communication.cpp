#include <stdio.h>
#include "communication.h"

BMS_BEGIN;


// Implementation of ServerLocator
ServerLocator::ServerLocator(){}

ServerLocator::~ServerLocator(){
    int i = 0;
    for (; i <= clustermap::self; i++)
        if (_node[i]) {
            PLIST* tempnode = _node[i];
            _node[i] = _node[i]->next;
            delete tempnode;
        }
}

PLIST* ServerLocator::setCMNode(const InputProtocol& iprctl_) {
    int i = iprctl_.type;

    PLIST* tempnode = _node[i];


    _node[i] = new PLIST();
    _node[i]->next = tempnode;

    _node[i]->node->m_type = iprctl_.type;
    strcpy(_node[i]->node->m_ip, iprctl_.ip);
    _node[i]->node->m_ip[strlen(iprctl_.ip)] = 0x00;
    _node[i]->node->m_port = iprctl_.port;
    _node[i]->node->m_protocol = iprctl_.protocol;

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

// Implementation of Communication

Communication::Communication () {
    _node = NULL;
}

Communication::Communication (clustermap::CMISNode* node_) {
    _node = node_;
}

    Communication::~Communication () {
        if (_node)
            delete _node;
    }



anet::Connection* Communication::makeConnection (SCHE::ConnectionPool* conn_pool) {
    char key[64];

    if(_node->m_protocol == clustermap::http) {
        snprintf(key, 64, "tcp:%s:%u", _node->m_ip, _node->m_port);
        return conn_pool->makeHttpConnection(key);
    } else if(_node->m_protocol == clustermap::tcp) {
        snprintf(key, 64, "tcp:%s:%u", _node->m_ip, _node->m_port);
        return conn_pool->makeTcpConnection(key);
    } else if(_node->m_protocol == clustermap::udp) {
        TERR("upd protocol is not sup_node->m_portted now.");
        return NULL;
    }
    TERR("unknown protocol.");
    return NULL;
}

anet::DefaultPacket* Communication::makePacket (char* query_) {
    // Build packet
    anet::DefaultPacket* packet = NULL;

    if(unlikely(!_node)) return NULL;
    if(_node->m_protocol == clustermap::http) {
        anet::HTTPPacket * p = new (std::nothrow) anet::HTTPPacket;
        if(unlikely(!p)) return NULL;
        p->setMethod(anet::HTTPPacket::HM_GET);
        if(query_ && strlen(query_) > 0) p->setURI(query_);
        else p->setURI("");
        p->addHeader("Accept", "*/*");
        p->addHeader("Connection", "Keep-Alive");
        packet = p;
    } else if(_node->m_protocol == clustermap::tcp) {
        anet::DefaultPacket * p = new (std::nothrow) anet::DefaultPacket;
        if(unlikely(!p)) return NULL;
        if(query_ && strlen(query_) > 0) p->setBody(query_, strlen(query_));
        else p->setBody("", 0);
        packet = p;
    } else if(_node->m_protocol == clustermap::udp) {
        //TODO: create udp packet
    }

    return packet;
}

ret_t Communication::sendPacket (anet::DefaultPacket* &return_packet, anet::DefaultPacket* packet, anet::Connection* conn = NULL) {

    if (conn == NULL)
        conn = _conn;

    if (unlikely(conn == NULL || packet == NULL)) {
        return r_enomem;
    }
    printf("Sending packet...\n");

    anet::Packet* tp_return_packet = NULL;
    if (_node->m_protocol == clustermap::tcp || _node->m_protocol == clustermap::http)
        tp_return_packet = this->tcpSendPacket(dynamic_cast<anet::TCPConnection* >(conn), packet);
    else            // UDP has no implementation.
        return r_failed;    


    return_packet = dynamic_cast<anet::DefaultPacket*>(tp_return_packet);

    if (!return_packet) {
        packet->free();
        conn->subRef();
        return r_failed;
    }

    conn->subRef();
    return r_succeed;
}

void Communication::setNode (clustermap::CMISNode* node_) {
    this->_node = node_;
}

anet::Packet* Communication::tcpSendPacket (anet::TCPConnection* tcp_conn, anet::DefaultPacket* packet) {
    anet::SynStub stub;

    // DEBUG
    /*
    int _res = -1;
    int opt;
    socklen_t len = sizeof(int);
    while (_res < 0)
        _res = getsockopt(tcp_conn->getSocket()->getSocketHandle(), SOL_SOCKET, SO_RCVBUF, &opt, &len);
    //printf("OPT: %d , LEN: %d\n". opt, len);
    std::cout << "OPT: " << opt << std::endl;
    */
    // END DEBUG

    /*
    bool conn_res = true;
    int err_no;
    do{
    conn_res = tcp_conn->writeData();
    if (!conn_res)
        return NULL;
    err_no = anet::Socket::getLastError();
    std::cout << err_no << std::endl;
    } while (err_no == EAGAIN || err_no == EWOULDBLOCK);
    */
    std::cout << tcp_conn->getSocket()->getSocketHandle() << std::endl;

    int opt = 50368, buf;
    socklen_t len = sizeof(int);
    setsockopt(tcp_conn->getSocket()->getSocketHandle(), SOL_SOCKET, SO_SNDBUF, &opt, len);
    getsockopt(tcp_conn->getSocket()->getSocketHandle(), SOL_SOCKET, SO_SNDBUF, &buf, &len);
    std::cout << "OPT: " << buf << std::endl;
    
    // phase 1 & phase 2
    if (true) {
        if (!tcp_conn->postPacket(packet, &stub, &stub)) 
            return NULL;
        tcp_conn->writeData();
    }
    else {
    int res = ::write(tcp_conn->getSocket()->getSocketHandle(), packet->getBody(), packet->getBodyLen());
    printf("write RES : %d \n", res);
    }
    getsockopt(tcp_conn->getSocket()->getSocketHandle(), SOL_SOCKET, SO_SNDBUF, &buf, &len);
    std::cout << "After send OPT: " << buf << std::endl;

    anet::Packet* return_packet = NULL;
    printf("Receving data");
    while (NULL == return_packet) {
        tcp_conn->readData();
        return_packet = stub.getReply();
    }
    printf("\n");
    /*
       int res = this->readPacketFromSocket(tcp_conn->getSocket(), return_packet);
       if (res < 0)
       return NULL;
       */
    return return_packet;
}

int Communication::readPacketFromSocket(anet::Socket* socket, anet::DefaultPacket* &packet) {
    int readCnt = 0;
    packet = new anet::DefaultPacket();
    char* buf = (char *) ::malloc(sizeof(char) * READ_WRITE_SIZE);
    int buf_size = READ_WRITE_SIZE;
    int buf_ptr = 0;
    int len;

    do{
        readCnt++;
        char* data = (char*) ::malloc(sizeof(char) * READ_WRITE_SIZE);

        len = socket->read(data, READ_WRITE_SIZE);
        if (len <= 0)
            break;
        if (len < READ_WRITE_SIZE)
            data[len] = 0x00;

        while (buf_size - buf_ptr < len) {
            buf_size *= 2;
            ::realloc(buf, sizeof(char) * buf_size);
        }
        memcpy(buf + buf_ptr, data, len);
        buf_ptr += len;

        ::free(data);
    } while (len == READ_WRITE_SIZE && readCnt < 10);

    packet->setBody(strdup(buf), buf_size);
    assert(packet->getBodyLen() == strlen(packet->getBody()));

    ::free(buf);
    return 0;
}
BMS_END;
