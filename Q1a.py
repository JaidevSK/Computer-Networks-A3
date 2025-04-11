from mininet.topo import Topo
from mininet.net import Mininet
from mininet.log import setLogLevel, info
from mininet.cli import CLI
from mininet.link import TCLink
# As given above, I first import all the necessary libraries



class A3Q1a(Topo):
    # Now, here I create a class that inherits from Topo
    # This class is used to create the topology that is given in Question 1
    def build(self):
        # This function is used to build the topology
        # The four switches are created
        s1 = self.addSwitch('s1') # Switch s1
        s2 = self.addSwitch('s2') # Switch s2
        s3 = self.addSwitch('s3') # Switch s3
        s4 = self.addSwitch('s4') # Switch s4

        # The eight hosts are created with the specified IP addresses
        h1 = self.addHost('h1', ip='10.0.0.2/24') # Host h1
        h2 = self.addHost('h2', ip='10.0.0.3/24') # Host h2
        h3 = self.addHost('h3', ip='10.0.0.4/24') # Host h3
        h4 = self.addHost('h4', ip='10.0.0.5/24') # Host h4
        h5 = self.addHost('h5', ip='10.0.0.6/24') # Host h5
        h6 = self.addHost('h6', ip='10.0.0.7/24') # Host h6
        h7 = self.addHost('h7', ip='10.0.0.8/24') # Host h7
        h8 = self.addHost('h8', ip='10.0.0.9/24') # Host h8

        # Then I add the links between the switches and hosts with the specified delays
        self.addLink(h1, s1, delay='5ms') # Link between h1 and s1
        self.addLink(h2, s1, delay='5ms') # Link between h2 and s1
        self.addLink(h3, s2, delay='5ms') # Link between h3 and s2
        self.addLink(h4, s2, delay='5ms') # Link between h4 and s2
        self.addLink(h5, s3, delay='5ms') # Link between h5 and s3
        self.addLink(h6, s3, delay='5ms') # Link between h6 and s3
        self.addLink(h7, s4, delay='5ms') # Link between h7 and s4
        self.addLink(h8, s4, delay='5ms') # Link between h8 and s4
        self.addLink(s1, s2, delay='7ms') # Link between s1 and s2
        self.addLink(s2, s3, delay='7ms') # Link between s2 and s3
        self.addLink(s3, s4, delay='7ms') # Link between s3 and s4
        self.addLink(s4, s1, delay='7ms') # Link between s4 and s1
        self.addLink(s1, s3, delay='7ms') # Link between s1 and s3



topo = A3Q1a() # I create the topology that is given in Question 1
net = Mininet(topo=topo, controller=None, link=TCLink) # I create the network with the topology that I created above, there is no controller
setLogLevel( 'info' ) # I set the log level to info
info( '*** Starting network\n' ) # I log that I am starting the network
net.start() # Then the network is started
CLI(net) # I start the CLI. I will perform the tests in the CLI. The corresponding commands are given in the Report
info("***Stopping network\n") # I log that I am stopping the network
net.stop() # I stop the network


# References:
# http://mininet.org/walkthrough/
# https://github.com/mininet/mininet/blob/master/examples/linuxrouter.py
# https://github.com/mininet/mininet/blob/master/examples/popen.py
