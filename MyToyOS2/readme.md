
Starting from tutorial:
http://wiki.osdev.org/Bare_Bones

+ Added raw I/O from serial line COM1.
  Ref.
      http://wiki.osdev.org/Serial_Ports
      https://en.wikibooks.org/wiki/Serial_Programming/8250_UART_Programming

TODO
- get current baud rate



Dev architecture
----------------


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


---------
Utilities
---------

Stream TCP packet  into serial
-----------------------------
Reads all TCP packets from <127.0.0.1> and forwards to 'serial_io'
unix domain file.

$ sudo socat IP4-RECVFROM:6,range=127.0.0.1/0,fork UNIX-CONNECT:serial_io


dummy receiver
--------------
$ socat UNIX-LISTEN:serial_io,ignoreeof EXEC:/home/fabio/Documents/ToyOS/MyToyOS2/dummyMiniWeb.sh

hexdum a binary string to stderr:
$ echo -n -e 'ciao a tutti' | hexdump -e '4/1 "%02X""\t"" "' -e '4/1 "%c""\n"' >&2
6369616F	 ciao
20612074	  a t
75747469	 utti


socat proxy
$ socat - UNIX-CONNECT:serial_io



Send handcrafted ip packet
--------------------------
$ sudo sendip 10.0.0.2 -p ipv4 -p tcp -td 8880

# TCP_FIN flag
$ sudo sendip 10.0.0.2 -p ipv4 -p tcp -td 8880 -tff 1


Receive TCP/IP packets
-----------------------
(from https://linux.die.net/man/7/raw)
"Receiving of all IP protocols via IPPROTO_RAW is not possible using raw sockets."
