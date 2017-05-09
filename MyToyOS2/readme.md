
Implements a mini-webserver that runs inside bare-metak machine (i.e. with no OS).

Starting from tutorial:
http://wiki.osdev.org/Bare_Bones

+ Added raw I/O from serial line COM1.





Dev architecture
----------------

```
  eth                    ip               tcp                tcp                                          tcp   +-----------+
 frames   +---------+   pkts  +-------+   pkts   +-------+   pkts   .-------------------.      +------+   pkts  |  miniweb  |
<-------> | eth0/lo | <-----> | Linux | <------> | socat | <-----> () unix domain socket ) <-> | vbox | <-----> |    on     |
          +---------+         +-------+          +-------+          '-------------------'      +------+   uart  | SantaNoOS |
                                                                                                                +-----------+

eth header + ip header + tcp header + payload
                         '------------------'
                               tcp packet
             '------------------------------'
                         ip packet
'-------------------------------------------'
                  eth frame
```

---------
Utilities
---------

Send handcrafted ip packet
--------------------------
```
$ sudo sendip 10.0.0.2 -p ipv4 -p tcp -td 8880

# TCP_FIN flag
$ sudo sendip 10.0.0.2 -p ipv4 -p tcp -td 8880 -tff 1
```

Receive TCP/IP packets
-----------------------
Use TUN device ...


Ref.
----
- http://wiki.osdev.org/Serial_Ports
- https://en.wikibooks.org/wiki/Serial_Programming/8250_UART_Programming
