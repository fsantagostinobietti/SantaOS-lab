
Goal
----
Learn to develop an educational Operating System.
I'll start from the simplest app running without an OS (MyToyOS1 prj) and I'll go on 
to more complex examples.


    
Projects
--------
MyToyOS1 - real-mode x86 mini (<512 byte) kernel. No bootloader needed.

MyToyOS2 - protected-mode x86 sample kernel loaded by GRUB.
           Implement a self-made basic web server application running on bare metal machine.
           Features:
            * app must respond to simple GET request in HTTP/1.0 with "hello world" message
            * x86 32 bit cpu
            * PC architecture: bare metal or virtual (vbox or vmware)
            * no OS, single process, single thread
            
MyToyOS3 - same as previous but with better dev structure.
