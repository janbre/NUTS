import socket, select, time
import thread

s = socket.socket()

host = ''
port = 1234
s.bind((host, port))

s.listen(5)

inputs = [s]


def send_large(thread_name, connection):
    for i in range(100):
        connection.send(str(i))
        time.sleep(1)


while True:
    rs, ws, es = select.select(inputs, [], [])
    for r in rs:
        if r is s:
            c, addr = s.accept()
            print 'Got connection from ', addr
            inputs.append(c)
        else:
            try:
                data = r.recv(1024)
                disconnected = not data
            except socket.error:
                disconnected = True

            if disconnected:
                print r.getpeername(), ' disconnected'
                inputs.remove(r)
            else:
                if data[:4] == 'help':
                    print 'received request for help'
                    r.send('yes, this is help?')
                    thread.start_new_thread( send_large, ('thread-1', r))
                else:
                    print 'computer says no'
                print data, len(data)
