def hello():
    print 'Hello from the reactor loop!'
    print 'Lately I feel like I\'m stuck'

from twisted.internet import reactor

reactor.callWhenRunning(hello)

print 'Starting the reactor.'
reactor.run()
