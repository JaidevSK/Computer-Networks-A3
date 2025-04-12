from mininet.net import Mininet
from mininet.node import RemoteController
from mininet.cli import CLI
from mininet.link import TCLink
from mininet.topo import Topo
# Firstly, I imported all the necessary modules from the Mininet library.
# Simulatneosuly, I also started a pox controller in a separate terminal using the same command as before.

class Q2(Topo):
    # I define the topology class Q2, which inherits from the Topo class in Mininet.
    def build(self):
        # I create 4 swithes as s1, s2, s3, and s4.
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        s3 = self.addSwitch('s3')
        s4 = self.addSwitch('s4')

        # I create 9 hosts as h1, h2, h3, h4, h5, h6, h7, h8, and h9.
        # Each host is assigned an IP address and a default route.
        h1 = self.addHost('h1', ip='10.1.1.2/24', defaultRoute='via 10.1.1.1') # I create host h1
        h2 = self.addHost('h2', ip='10.1.1.3/24', defaultRoute='via 10.1.1.5') # I create host h2
        h3 = self.addHost('h3', ip='172.16.10.4', defaultRoute='via 171.16.10.10') # I create host h3
        h4 = self.addHost('h4', ip='172.16.10.5', defaultRoute='via 171.16.10.10') # I create host h4
        h5 = self.addHost('h5', ip='172.16.10.6', defaultRoute='via 171.16.10.10') # I create host h5
        h6 = self.addHost('h6', ip='172.16.10.7', defaultRoute='via 171.16.10.10') # I create host h6
        h7 = self.addHost('h7', ip='172.16.10.8', defaultRoute='via 171.16.10.10') # I create host h7
        h8 = self.addHost('h8', ip='172.16.10.9', defaultRoute='via 171.16.10.10') # I create host h8
        h9 = self.addHost('h9', ip='172.16.10.10') # I create host h9. I will use this host as a NAT.

        # I add links between the switches and hosts with specified delays in the main sub-net.
        self.addLink(s1, s2, delay='7ms') 
        self.addLink(s2, s3, delay='7ms')
        self.addLink(s3, s4, delay='7ms')
        self.addLink(s4, s1, delay='7ms')
        self.addLink(s1, s3, delay='7ms')
        self.addLink(h3, s2, delay='5ms')
        self.addLink(h4, s2, delay='5ms')
        self.addLink(h5, s3, delay='5ms')
        self.addLink(h6, s3, delay='5ms')
        self.addLink(h7, s4, delay='5ms')
        self.addLink(h8, s4, delay='5ms')

        # I add links between the hosts and the switches with specified delays in the small sub-net.
        self.addLink(h9, s1, delay='5ms')            
        self.addLink(h9, h1, delay='5ms', intfName1 = 'h9-eth1', intfName2 = 'h1-eth0')
        self.addLink(h9, h2, delay='5ms', intfName1 = 'h9-eth2', intfName2 = 'h2-eth0')

if __name__ == '__main__':
    topo = Q2() # I create an instance of the Q2 topology class.
    net = Mininet(topo=topo, link=TCLink, controller=None) # I create a Mininet instance with the Q2 topology and TCLink as the link type.
    net.addController('pox', controller=RemoteController, ip='127.0.0.1', port=6633) # I add a remote controller (POX) to the Mininet instance.
    net.start() # I start the Mininet instance.

    h9 = net.get('h9') # I get the host h9 from the Mininet instance.
    h9.cmd("ip addr add 172.16.10.10/24 dev h9-eth0")  # I assign an IP address to h9's eth0 interface as Public IP (in our setup)
    h9.cmd("ip addr add 10.1.1.1/24 dev h9-eth1")  # I assign an IP address to h9's eth1 interface as Private IP (in our setup)
    h9.cmd("ip addr add 10.1.1.5/24 dev h9-eth2")  # I assign an IP address to h9's eth2 interface as Private IP (in our setup)
    h9.cmd("sysctl net.ipv4.ip_forward=1") # I enable IP forwarding on h9 to allow it to act as a router.

    h1 = net.get('h1') # I get the host h1 from the Mininet instance.
    h1.cmd("ip route del default") # I delete the default route for h1.
    h1.cmd("ip route add default via 10.1.1.1 dev h1-eth0") # I add a new default route for h1 via h9's eth1 interface.

    h2 = net.get('h2') # I get the host h2 from the Mininet instance.
    h2.cmd("ip route del default") # I delete the default route for h2.
    h2.cmd("ip route add default via 10.1.1.5 dev h2-eth0") # I add a new default route for h2 via h9's eth2 interface.

    # Now, I set the NAT rules
    h9.cmd("iptables -t nat -A POSTROUTING -o h9-eth0 -s 10.1.1.0/24 -j SNAT --to-source 172.16.10.10") # This rule allows the hosts in the private network to access the public network.
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -p icmp -d 172.16.10.10 -j DNAT --to-destination 10.1.1.2") # This rule allows ICMP packets destined for the public IP to be forwarded to h1.
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -p icmp -d 172.16.10.10 -j DNAT --to-destination 10.1.1.3") # This rule allows ICMP packets destined for the public IP to be forwarded to h2.
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -p tcp --dport 8080 -d 172.16.10.10 -j DNAT --to-destination 10.1.1.2:80") # Assuming h1 listens on port 80, this rule allows TCP packets destined for port 8080 on the public IP to be forwarded to h1.
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -p tcp --dport 9090 -d 172.16.10.10 -j DNAT --to-destination 10.1.1.3:80") # Assuming h2 listens on port 80, this rule allows TCP packets destined for port 9090 on the public IP to be forwarded to h2.

    # Forwarding rule
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth1 -j ACCEPT") # This rule allows packets from the public network to be forwarded to the private network (H1)
    h9.cmd("iptables -A FORWARD -i h9-eth1 -o h9-eth0 -j ACCEPT") # This rule allows packets from the private network to be forwarded to the public network (H1)
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth2 -j ACCEPT") # This rule allows packets from the public network to be forwarded to the private network (H2)
    h9.cmd("iptables -A FORWARD -i h9-eth2 -o h9-eth0 -j ACCEPT") # This rule allows packets from the private network to be forwarded to the public network (H2)
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth1 -m state --state NEW -j ACCEPT") # This rule allows new connections from the public network to be forwarded to the private network (H1)
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth2 -m state --state NEW -j ACCEPT") # This rule allows new connections from the public network to be forwarded to the private network (H2)


    for host in ['h3', 'h4', 'h5', 'h6', 'h7', 'h8']: # I loop through the hosts h3 to h8 and add a route to the main sub-net. This can be commented out if we want to ping via IP as I told in report.
        net.get(host).cmd(f"ip route add 10.1.1.0/24 via 172.16.10.10") # This is optional, but it allows the hosts in the small sub-net to communicate with the hosts in the main sub-net.

    CLI(net) # I start the Mininet command-line interface (CLI) to allow interaction with the network.
    net.stop() # I stop the Mininet instance when the CLI is exited.

## References:
# https://linux.die.net/man/8/iptables
# https://sudamtm.medium.com/iptables-a-comprehensive-guide-276b8604eff1
# https://linux.die.net/man/8/ip
# https://phoenixnap.com/kb/linux-ip-command-examples
# man pages for ip, iptables as well as linux documentation
