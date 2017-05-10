
Implements a mini-webserver that runs inside bare-metak machine (i.e. with no OS).

Starting from tutorial:
http://wiki.osdev.org/Bare_Bones

+ Added raw I/O from serial line COM1.





Dev architecture
----------------

```
  eth                  tcp/ip                 tcp/ip                                      tcp/ip  +-----------+
 frames   +---------+   pkts  +------------+   pkts   .-------------------.      +------+   pkts  |  miniweb  |
<-------> | TUN dev | <-----> | tun2serial | <-----> () unix domain socket ) <-> | vbox | <-----> |    on     |
          +---------+         +------------+          '-------------------'      +------+   uart  | SantaNoOS |
                                                                                                  +-----------+

eth header + ip header + tcp header + payload
                         '------------------'
                               tcp packet
             '------------------------------'
                         ip packet
'-------------------------------------------'
                  eth frame
```

How to build executable image
-----------------------------
```
$ ./compile.sh
```

'mytoyos.iso' image is produced.
It can be run in a VM like QEMU, VMware or Virtual Box.
In this example I use Virtual Box because serial port (uart) can be mapped with a unix domain socket in the host system.

How to execute and test miniweb application
-------------------------------------------
1) cretate TUN interface (must be done only once after every reboot)
```
$ ./createTUN.sh
```
    
2) start execution of 'mytoyos.iso' in vbox.
```
$ ./runMyToyOS.sh
```
    
3) execute proxy application that sends tcp/ip packets in the unix domain socket, and vice-versa.
```
$ ./eth2serial.sh
```

4) finally test web application with a simple GET request
```
$ curl --noproxy 10.0.0.2  10.0.0.2:8880
```
It should output something like this:
```
<html><body><p>Hello World!</p></body></html>
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
