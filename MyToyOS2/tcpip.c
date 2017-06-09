/*
    Simplest TCP/IP stack.
*/




#include <stdint.h>
#include <stdbool.h>
#include "tcpip.h"
#include "serial.h"
#include "tty.h"
#include "std.h"
#include "stdio.h"

/*
 * TCP/IP utils
 */


/* converts a word (2 bytes) from network byte order
to native byte order. */
uint16_t toNative16(uint8_t net[]) {
    // Intel uses little endian byte order
    uint32_t native = 0x00000000;
    native = net[0];
    native = native<<8 | net[1];
    return native;
}

/* converts a dword (4 bytes) from network byte order
to native byte order. */
uint32_t toNative32(uint8_t net[]) {
    // Intel uses little endian byte order
    uint32_t native = 0x00000000;
    native = net[0];
    native = native<<8 | net[1];
    native = native<<8 | net[2];
    native = native<<8 | net[3];
    return native;
}

/* converts a word (2 bytes) from native byte order
to network byte order. */
void toNetwork16(const uint16_t val, uint8_t net[]) {
    net[1] = val & 0x00FF;
    net[0] = (val>>8) & 0x00FF;
}

/* converts a dword (4 bytes) from native byte order
to network byte order. */
void toNetwork32(const uint32_t val, uint8_t net[]) {
    net[3] = val & 0x00FF;
    net[2] = (val>>8) & 0x00FF;
    net[1] = (val>>16) & 0x00FF;
    net[0] = (val>>24) & 0x00FF;
}


/* generic checksum algo */
uint16_t _checksum(uint32_t initSum32, uint8_t data[], int sz) {
    // compute 32-bit sum
    uint32_t chksum32 = initSum32;
    int isOdd = sz%2;
    int szEven = sz-sz%2;
    for (int i=0; i<szEven ;i=i+2)
        chksum32 += toNative16(data+i);
    if (isOdd) {
        // 2 bytes padding
        uint8_t lastData[2] = {data[szEven], 0x00};
        chksum32 += toNative16(lastData);
    }
    // compute 1's complement 16-bit sum
    uint32_t chksum16 = (chksum32>>16) + (chksum32&0x0000FFFF);
    // compute 1's complement
    chksum16 = ~chksum16;
    return chksum16;
}

uint16_t checksumIp(uint8_t data[], int sz) {
    return _checksum(0x00000000, data, sz);
}

/*
 * IP
 */

bool parseRawIpPacket(IP *ip, uint8_t raw[], int sz) {
    int idx = 0; // raw array cursor
    
    uint8_t buf8 = raw[idx]; idx += 1;
    ip->ver        = buf8 >> 4;
    ip->ihl        = buf8 & 0x0F;
    idx += 1;   // skip DSCP/ECN
    ip->len        = toNative16(raw+idx); idx += 2;
    ip->id         = toNative16(raw+idx); idx += 2;
    uint16_t buf16 = toNative16(raw+idx); idx += 2;
    ip->flags      = buf16 >> 13;
    ip->flagOffset = buf16 & 0x1FFF;
    ip->ttl        = raw[idx]; idx += 1;
    ip->protocol   = raw[idx]; idx += 1;
    ip->checksum   = toNative16(raw+idx); idx += 2;
    ip->srcAddr    = toNative32(raw+idx); idx += 4;
    ip->dstAddr    = toNative32(raw+idx); idx += 4;
    idx += ip->ihl * 4 - 20;  // skip OPTIONS (if any)
    ip->payloadSz  = ip->len - ip->ihl * 4;
    ip->payload    = raw+idx; idx += ip->payloadSz;  // payload size  
    
    // checksum of ip header (first 20 bytes of raw data)
    if ( checksumIp(raw, 20) != 0x0000 ) {
        warning("IP cheksum failed. Packet dropped!\n");
        return false;
    }
    return (idx==sz) ?true :false;
}

void buildRawIp(const IP *ip, uint8_t raw[], int *pRawSz) {
    uint8_t *buf = raw;
    
    // header
    *buf = ip->ver<<4 | ip->ihl;     buf += 1;
    *buf = 0x00;                     buf += 1;    // ignore DSCP/ECN
    toNetwork16(ip->len, buf);       buf += 2;
    toNetwork16(ip->id, buf);        buf += 2;
    uint16_t buf16 = ip->flags<<13 | ip->flagOffset;
    toNetwork16(buf16, buf);         buf += 2;
    *buf = ip->ttl;                  buf += 1;
    *buf = ip->protocol;             buf += 1;
    toNetwork16(ip->checksum, buf);  buf += 2;
    toNetwork32(ip->srcAddr, buf);   buf += 4;
    toNetwork32(ip->dstAddr, buf);   buf += 4;
    buf += ip->ihl * 4 - 20;  // ignore OPTIONS
    
    // payload
    memcpy(buf, ip->payload, ip->payloadSz); buf += ip->payloadSz;
    
    *pRawSz = buf-raw;
}




/*
 * TCP
 */


uint16_t checksumTcp(uint32_t srcAddr, uint32_t dstAddr, uint8_t protocol, uint8_t data[], int sz) {    
    uint32_t sum32 = (srcAddr>>16) + (srcAddr&0xFFFF) + 
                     (dstAddr>>16) + (dstAddr&0xFFFF) +  
                     (uint32_t)protocol +
                     ((uint32_t)sz&0xFFFF);
    return _checksum(sum32, data, sz);
}

/* extract TCP data struct */
void parseRawTcpPacket(TCP *tcp, uint8_t raw[], int sz) {
    int idx = 0; // raw array cursor
        
    tcp->srcPort = toNative16(raw+idx); idx += 2;
    tcp->dstPort = toNative16(raw+idx); idx += 2;
    tcp->seq     = toNative32(raw+idx); idx += 4;
    tcp->ack     = toNative32(raw+idx); idx += 4;
    tcp->offset  = raw[idx]>>4; idx += 1;
    tcp->flags   = raw[idx]; idx += 1;
    tcp->winSz   = toNative16(raw+idx); idx += 2;
    tcp->chksum  = toNative16(raw+idx); idx += 2;
    tcp->urgPtr  = toNative16(raw+idx); idx += 2;
    idx += tcp->offset * 4 - 20;    // skip options
    tcp->payloadSz = sz - tcp->offset * 4;
    tcp->payload = raw+idx; idx += tcp->payloadSz;
}

bool validateTcpPacket(const IP *ip, uint8_t tcpRaw[], int sz) {
    // tcp checksum
    if ( checksumTcp(ip->srcAddr, ip->dstAddr, ip->protocol, tcpRaw, sz) != 0x0000 ) {
        warning("IP packet dropped: TCP cheksum failed!\n", "dummy");
        return false;
    }
    
    return true;
}


void buildRawTcp(const TCP *tcp, uint8_t raw[], int *pRawSz) {
    uint8_t *buf = raw;
    toNetwork16(tcp->srcPort, buf); buf += 2;
    toNetwork16(tcp->dstPort, buf); buf += 2;
    toNetwork32(tcp->seq,     buf); buf += 4;
    toNetwork32(tcp->ack,     buf); buf += 4;
    *buf = tcp->offset<<4;          buf += 1;
    *buf = tcp->flags;              buf += 1;
    toNetwork16(tcp->winSz,   buf); buf += 2;
    toNetwork16(tcp->chksum,  buf); buf += 2;
    toNetwork16(tcp->urgPtr,  buf); buf += 2;
    buf += tcp->offset * 4 - 20; // ignore OPTIONS
    memcpy(buf, tcp->payload, tcp->payloadSz); buf += tcp->payloadSz;
    
    *pRawSz = (buf-raw);
}



/*
    basic driver for serial line (TODO put in another file?)
 */
 
/* static global buffers.
   Size must be greater than MTU defined by TUN device (usually 1500) */
static uint8_t inRawBuf[2048];  // rx buffer

static const char MAGIC_NUMBER[3] = {0x0F, 0xAB, 0x10};

void readRawPacket(uint8_t **pBuf, int *pBufSz) {
    // search for magic number
    char prolog_magic[3] = {0x00, 0x00, 0x00};
    while ( bcmp(prolog_magic, MAGIC_NUMBER, sizeof(MAGIC_NUMBER)) != 0 ) {
        // read one more char and update 'prolog_magic'
        prolog_magic[0] = prolog_magic[1];
        prolog_magic[1] = prolog_magic[2];
        prolog_magic[2] = read_serial();
    }
    // found MAGIC_NUMBER
    
    // read packet size (big endian order)
    int size = ((unsigned char)read_serial())<<8 | ((unsigned char)read_serial());
    *pBufSz = size;
    
    // read whole payload into buffer
    for (int i=0; i<size ;++i)
        inRawBuf[i] = read_serial();
    *pBuf = inRawBuf;
}

void sendRawPacket(const uint8_t *buf, const int bufSz) {
    // send magic number 
    for (unsigned int i=0; i<sizeof(MAGIC_NUMBER) ;++i)
        write_serial((char)MAGIC_NUMBER[i]);
    // send packet size (2 bytes in big endian order)
    write_serial((char) ((bufSz>>8)&0x00FF));
    write_serial((char) (bufSz&0x00FF));
    
    // send actual packet
    for (int i=0; i<bufSz ;++i)
        write_serial((char)buf[i]);
}




void nextTcpIpPacket(IP *pIp, TCP *pTcp) {
    uint8_t *buf;
    int bufSz;
    while (true) {
        readRawPacket(&buf, &bufSz);
        if ( parseRawIpPacket(pIp, buf, bufSz)==false )
            continue;   // drop packet
        if ( pIp->ver!=IPv4 ) {
            warning("IP packet dropped: only IPv4 is supported\n");
            continue;   // drop packet
        }
        if ( pIp->protocol!=PROTOCOL_TCP ) {
            warning("IP packet dropped: only TCP packets are supported\n");
            continue;   // drop packet
        }
        if ( validateTcpPacket(pIp, pIp->payload, pIp->payloadSz)==false )
            continue;   // drop packet
        parseRawTcpPacket(pTcp, pIp->payload, pIp->payloadSz);
        
        // exit with valid TCP IP structs
        return;
    }
}


void buildRawTcpIpPacket(  const IP *lastIp, const TCP *lastTcp,                           // inputs
                           const uint8_t flags, uint8_t payload[], const int payloadSz,    // 
                           uint8_t raw[], int *pRawSz          // output
                      ) {
    TCP tcp;
    tcp.srcPort   = lastTcp->dstPort;
    tcp.dstPort   = lastTcp->srcPort;
    tcp.seq       = lastTcp->ack;
    // compute ack number
    int ackN = lastTcp->payloadSz;
    if ( ackN==0 )
        ackN = 1;
    tcp.ack       = lastTcp->seq + ackN;
    tcp.offset    = 5;     // 20 bytes header size (no options)
    tcp.flags     = flags;
    tcp.winSz     = 0x7210;   // generic value
    tcp.chksum    = 0x0000;    // initial value to update
    tcp.urgPtr    = 0x0000;
    tcp.payload   = payload;
    tcp.payloadSz = payloadSz;
    
    IP ip; 
    ip.ver        = IPv4;
    ip.ihl        = 5; // header length
    ip.len        = 40 + payloadSz;     // 20 (ip header) + 20 (tcp header) + (tcp payload)
    ip.id         = 0x0100;    // should be unique
    ip.flags      = FLAGS_DF;
    ip.flagOffset = 0;
    ip.ttl        = 0x40;
    ip.protocol   = PROTOCOL_TCP;
    ip.checksum   = 0x0000;  // initial value to update
    ip.srcAddr    = lastIp->dstAddr;
    ip.dstAddr    = lastIp->srcAddr;
    ip.payload    = NULL;    // initial value to update with tcp content
    ip.payloadSz  = 0;
    
    
    // update tcp cheksum
    uint8_t rawTcp[2048];
    int size;
    buildRawTcp(&tcp, rawTcp, &size);
    tcp.chksum = checksumTcp(ip.srcAddr, ip.dstAddr, ip.protocol, rawTcp, size);
    buildRawTcp(&tcp, rawTcp, &size);
    ip.payload   = rawTcp;
    ip.payloadSz = size;
    
    // update ip checksum
    uint8_t rawIp[2048];
    buildRawIp(&ip, rawIp, &size);
    ip.checksum = checksumIp(rawIp, ip.ihl * 4);
    
    // build final raw packet
    buildRawIp(&ip, raw, pRawSz);
}


void threeWayHandshake() {
    info("waiting for a new 3-way handshake\n");
    IP ip;
    TCP tcp;
    while (true) {
        nextTcpIpPacket(&ip, &tcp);
        if ( tcp.flags!=FLAG_SYN ) {
            info("TCP packet dropped: expected a SYN flag\n\n");
            continue;
        }
    
        info("preparing SYN/ACK response\n");
        uint8_t raw[2048];
        int size;
        buildRawTcpIpPacket( &ip, &tcp,
                             FLAG_SYN|FLAG_ACK, 
                             NULL, 0,    // no payload
                             raw, &size          // output
                          );        
        sendRawPacket(raw, size);
        
        nextTcpIpPacket(&ip, &tcp);
        if ( tcp.flags!=FLAG_ACK ) {
            info("TCP packet dropped: expected a ACK flag\n\n");
            continue;
        }
        
        info("3-way handshake completed successfully\n");
        return;
    }
}

void fourWayClose( const IP *lastIp, const TCP *lastTcp ) {
    uint8_t raw[2048];
    int size;
    
    info("starting 4-way close\n");
    buildRawTcpIpPacket( lastIp, lastTcp,
                         FLAG_FIN, 
                         NULL, 0,    // no payload
                         raw, &size          // output
                        );
    sendRawPacket(raw, size);
    
    IP ip;
    TCP tcp;
    info("waiting for FIN\n");
    while (true) {
        nextTcpIpPacket(&ip, &tcp);
        if ( tcp.flags & FLAG_FIN )
            break;
    }
    
    info("close with final ACK\n");
    buildRawTcpIpPacket( &ip, &tcp,
                         FLAG_ACK, 
                         NULL, 0,    // no payload
                         raw, &size          // output
                        );
    sendRawPacket(raw, size);
}



