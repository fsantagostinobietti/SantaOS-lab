VMNAME="MyToyOS2-test"

# test if VM is already present
VBoxManage showvminfo $VMNAME 2>/dev/null >/dev/null
if [ $? -ne 0 ];then
    # create new VM image
    VBoxManage createvm --name $VMNAME --register
    # add memory and map serial interface to unix domain socket
    VBoxManage modifyvm $VMNAME --memory 64 --vram 8 --cpus 1 --uart1 0x3F8 4 --uartmode1 server $(pwd)/serial_io
    # add storage controller ...
    VBoxManage storagectl $VMNAME --name IDE --add ide --bootable on
    # ... and dvd iso 
    VBoxManage storageattach $VMNAME --storagectl IDE --port 1 --device 1 --type dvddrive --medium $(pwd)/mytoyos.iso 
fi

# start image
VBoxManage startvm $VMNAME
