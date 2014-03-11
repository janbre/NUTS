from twisted.internet import protocol, reactor
import time

class Echo(protocol.Protocol):
    def dataReceived(self, data):
        f = open('webclient.html', 'r')
        print "data received, echoing"
       # self.transport.write(data*10)
        x = f.readlines()
        self.transport.write(''.join(x))

class EchoFactory(protocol.Factory):
    def buildProtocol(self, addr):
        return Echo()

reactor.listenTCP(1234, EchoFactory())
reactor.run()
