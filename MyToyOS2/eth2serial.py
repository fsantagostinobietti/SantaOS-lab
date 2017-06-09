''' 
Reads all TCP (protocol 6) packets generated from <127.0.0.1> (localhost) and forwards to script:
Reads tcp/ip packets from TUN device (no frame headers)



Pre-requisite: having a TUN device called 'mytun':
    # create 'mytun' tun device with grants for 'fabio':'fabio' user     
     $ sudo ip tuntap add dev mytun mode tun user fabio group fabio
     
    # bring device up (by default is down after creation)
     $ sudo ip link set mytun up

    # create route for 10.0.0.1/24 range
     $ sudo ip addr add 10.0.0.1/24 dev mytun
     
    # show device info
     $ ip link show dev mytun
 
In another shell:
 $ curl --noproxy 10.0.0.2  10.0.0.2:8880
 
Route packets from 'eth0' to 'mytun':
     # Allow traffic initiated from VPN to access LAN
    iptables -I FORWARD -i mytun -o eth0 \
         -s 10.0.0.0/24 -d 192.168.0.0/24 \
         -m conntrack --ctstate NEW -j ACCEPT

    # Allow established traffic to pass back and forth
    iptables -I FORWARD -m conntrack --ctstate RELATED,ESTABLISHED \
         -j ACCEPT

 
Ref.
    TUN/TAP tutorial - http://backreference.org/2010/03/26/tuntap-interface-tutorial/
    TUN/TAP in python - https://pypi.python.org/pypi/python-pytun/2.2.1
    
'''

from pytun import TunTapDevice, IFF_TUN, IFF_NO_PI
import socket
import sys
import threading
import signal
import struct

# constants
SRV_SOCKET = './serial_io'
TUN_DEVICE = 'mytun'

def u2B( num, nbytes ):
    ''' convert unsigned to byte string '''
    if nbytes==1:
        return struct.pack("!B", num)
    if nbytes==2:
        return struct.pack("!H", num) # 2 bytes, big-endian
    if nbytes==4:
        return struct.pack("!I", num) # 4 bytes, big-endian


def B2u( bb ):
    ''' convert byte string to unsigned  '''
    if len(bb)==1:
        return struct.unpack("!B", bb)[0]    # 1 byte
    if len(bb)==2:
        return struct.unpack("!H", bb)[0]    # 2 bytes, big-endian
    if len(bb)==4:
        return struct.unpack("!I", bb)[0]    # 4 bytes, big-endian
    raise NotImplementedError
        
def connectToSocket():
    # Create a UDS socket
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    
    # Connect the socket to the port where the server is listening
    print '! connecting to %s' % SRV_SOCKET
    try:
        sock.connect(SRV_SOCKET)
    except socket.error, msg:
        print >>sys.stderr, msg
        sys.exit(1)
    return sock
    
    
 
def connectToTun():
    tun = TunTapDevice(name=TUN_DEVICE, flags=IFF_TUN|IFF_NO_PI)
    # TUN info
    print 'TUN'
    print '  name:     ' + tun.name
    print '  src addr: ' + tun.addr
    print '  dst addr: ' + tun.dstaddr
    print '  netmask:  ' + tun.netmask
    print '  mtu:      ' + str(tun.mtu)
    print ''    
    return tun


class tun2socket (threading.Thread):
    def __init__(self, tun, sock):
        threading.Thread.__init__(self)
        self.tun = tun
        self.sock = sock
    def run(self):
        print '? tun->socket thread active'
        while True:
            pkt = self.tun.read(self.tun.mtu)
            # prepare a prolog (magic number + packet size) used by receiver to
            # detect the end of packet
            prolog = b'\x0F\xAB\x10' + u2B(len(pkt),2)
            # send prolog + packet
            self.sock.sendall(prolog + pkt)

class socket2tun (threading.Thread):
    def __init__(self, sock, tun):
        threading.Thread.__init__(self)
        self.tun = tun
        self.sock = sock
    def run(self):
        print '? socket->tun thread active'
        pkt = ''
        while True:
            pkt += self.sock.recv(2048)
            if len(pkt)>=5:
                # skip magic number
                size = B2u( pkt[3:5] )
                pkt = pkt[5:]   # strip prolog
                while len(pkt)<size:
                    pkt += self.sock.recv(2048)
                self.tun.write(pkt[:size])  # send packet
                pkt = pkt[size:]    # remove packet sent
                
                



'''
    main
'''
def main():
    tun = connectToTun()
    sock = connectToSocket()
    
    thread1 = tun2socket(tun, sock)
    thread1.setDaemon(True)
    thread2 = socket2tun(sock, tun)
    thread2.setDaemon(True)
    
    # start threads
    thread1.start()
    thread2.start()
    
    # wait (forever) to join
    while True:
        signal.pause() 
    
        
if __name__ == "__main__":
    # execute only if run as a script
    main()   
