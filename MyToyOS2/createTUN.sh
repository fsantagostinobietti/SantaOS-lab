# create 'mytun' tun device with grants for 'fabio':'fabio' user     
sudo ip tuntap add dev mytun mode tun user fabio group fabio
 
# bring device up (by default is down after creation)
sudo ip link set mytun up

# create route for 10.0.0.1/24 range
sudo ip addr add 10.0.0.1/24 dev mytun
 
# show device info
ip link show dev mytun
