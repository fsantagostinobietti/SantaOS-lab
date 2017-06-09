#ifndef _TCPIP_H_
#define _TCPIP_H_

/*
 * IP
 */

#define IP_PKT_MAGIC_NUMBER 0x0FAB10

#define IPv4   4      
#define PROTOCOL_TCP  6
#define FLAGS_DF   2    // don't fragment
#define FLAGS_MF   4    // more fragments
    
typedef struct IP {
    uint8_t  ver;    // 4 for IPv4
    uint8_t  ihl;    // header length
    // uint8_t DSCP/ECN ignored
    uint16_t len;
    uint16_t id;
    uint8_t  flags;
    uint16_t flagOffset;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t srcAddr;
    uint32_t dstAddr;
    // uint8_t[] OPTIONS  ignored
    int payloadSz;
    uint8_t* payload;
} IP;


/*
 * TCP
 */

typedef struct TCP {
    uint16_t  srcPort;
    uint16_t  dstPort;
    uint32_t  seq;  // sequence number
    uint32_t  ack;  // ack number
    uint8_t   offset;   // size of header in 32-bit words
    uint8_t   flags;
    uint16_t  winSz;    // window size
    uint16_t  chksum;
    uint16_t  urgPtr;   // urgent pointer
    // uint8_t[] OPTIONS  ignored
    int payloadSz;
    uint8_t* payload;
} TCP;

#define FLAG_FIN  0x01
#define FLAG_SYN  0x02
#define FLAG_RST  0x04
#define FLAG_PSH  0x08
#define FLAG_ACK  0x10
#define FLAG_URG  0x20


void nextTcpIpPacket(IP *pIp, TCP *pTcp);
void threeWayHandshake();
void buildRawTcpIpPacket( const IP *lastIp, const TCP *lastTcp, const uint8_t flags, uint8_t payload[], const int payloadSz, uint8_t raw[], int *pRawSz );
void sendRawPacket(const uint8_t *buf, const int bufSz);
void fourWayClose( const IP *lastIp, const TCP *lastTcp );


#endif /* _TCPIP_H_ */
