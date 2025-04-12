from mininet.net import Mininet
from mininet.node import RemoteController, Host
from mininet.cli import CLI
from mininet.link import TCLink
from mininet.topo import Topo
# This is exactly same as Q2.py except a switch and therefore, I have not added redundant comments. Only the changes are mentioned.

class Q2(Topo):
    def build(self):
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        s3 = self.addSwitch('s3')
        s4 = self.addSwitch('s4')
        s5 = self.addSwitch('s5')  # I have added a new switch here to ensure good connectivity in the small sub-net.
        
        h1 = self.addHost('h1', ip='10.1.1.2/24', defaultRoute='via 10.1.1.1')
        h2 = self.addHost('h2', ip='10.1.1.3/24', defaultRoute='via 10.1.1.1')
        h3 = self.addHost('h3', ip='172.16.10.4', defaultRoute='via 172.16.10.10')
        h4 = self.addHost('h4', ip='172.16.10.5', defaultRoute='via 172.16.10.10')
        h5 = self.addHost('h5', ip='172.16.10.6', defaultRoute='via 172.16.10.10')
        h6 = self.addHost('h6', ip='172.16.10.7', defaultRoute='via 172.16.10.10')
        h8 = self.addHost('h8', ip='172.16.10.8', defaultRoute='via 172.16.10.10')
        h7 = self.addHost('h7', ip='172.16.10.9', defaultRoute='via 172.16.10.10')
        h9 = self.addHost('h9', ip='172.16.10.10', defaultRoute='via 172.16.10.10')  

        self.addLink(s1, s2, delay='7ms')
        self.addLink(s2, s3, delay='7ms')
        self.addLink(s3, s4, delay='7ms')
        self.addLink(s4, s1, delay='7ms')
        self.addLink(s1, s3, delay='7ms')

        self.addLink(h9, s1, delay='5ms')            
        self.addLink(h9, s5, delay='5ms') # I will connect h9 with h1,h2 through s5
        self.addLink(h1, s5, delay='5ms') # I will connect h1 with h9 through s5
        self.addLink(h2, s5, delay='5ms') # I will connect h2 with h9 through s5

        self.addLink(h3, s2, delay='5ms')
        self.addLink(h4, s2, delay='5ms')
        self.addLink(h5, s3, delay='5ms')
        self.addLink(h6, s3, delay='5ms')
        self.addLink(h7, s4, delay='5ms')
        self.addLink(h8, s4, delay='5ms')

if __name__ == '__main__':
    topo = Q2()
    net = Mininet(topo=topo, link=TCLink, controller=None)
    net.addController('pox', controller=RemoteController, ip='127.0.0.1', port=6633)
    net.start()

    h9 = net.get('h9')
    h9.cmd("ip addr add 10.1.1.1/24 dev h9-eth1")  
    h9.cmd("ip addr add 172.16.10.10/24 dev h9-eth0")  
    h9.cmd("sysctl net.ipv4.ip_forward=1")

    h9.cmd("iptables -t nat -A POSTROUTING -o h9-eth0 -s 10.1.1.0/24 -j SNAT --to-source 172.16.10.10")
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -p icmp -d 172.16.10.10 -j DNAT --to-destination 10.1.1.2")
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -p icmp -d 172.16.10.10 -j DNAT --to-destination 10.1.1.3")
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -p tcp --dport 8080 -d 172.16.10.10 -j DNAT --to-destination 10.1.1.2:8080")
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -p tcp --dport 9090 -d 172.16.10.10 -j DNAT --to-destination 10.1.1.3:9090")
    h9.cmd("iptables -t nat -A PREROUTING -i h9-eth0 -d 172.16.10.10 -j DNAT --to-destination 10.1.1.2")
    

    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth1 -m state --state RELATED,ESTABLISHED -j ACCEPT")
    h9.cmd("iptables -A FORWARD -i h9-eth1 -o h9-eth0 -j ACCEPT")
    h9.cmd("iptables -A FORWARD -i h9-eth0 -o h9-eth1 -m state --state NEW -j ACCEPT")

    for host in ['h3', 'h4', 'h5', 'h6', 'h7', 'h8']:
        net.get(host).cmd(f"ip route add 10.1.1.0/24 via 172.16.10.10")

    CLI(net)
    net.stop()
