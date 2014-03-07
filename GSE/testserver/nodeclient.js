var net = require("net");
var socket = net.createConnection(50000, "127.0.0.1");
socket.setEncoding("utf8");
socket.on("connect", function(){
	socket.on("data", function(data){
		socket.write(data);
	});
	socket.write("Echo Client\r\n");
});
socket.on("end", function(){
	// can be done else where, is similar to http .end("data", encoding);
	socket.end();
	// cleans up the socket.
	socket.destroy();
});
