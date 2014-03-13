from twisted.internet import protocol, reactor
import time, random, string

class Echo(protocol.Protocol):
    def dataReceived(self, data):
        f = open('webclient.html', 'r')
        print "data received, sending random bytes"
        #self.transport.write(data*10)
        x = ''
        for i in range(255):
            x += random.choice(string.ascii_letters)
        self.transport.write(x)

class EchoFactory(protocol.Factory):
    def buildProtocol(self, addr):
        return Echo()

reactor.listenTCP(1234, EchoFactory())
reactor.run()
