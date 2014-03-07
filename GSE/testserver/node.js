var http=require('http');
var util=require('util');
var querystring=require('querystring');
var net = require('net');

var server=http.createServer(function(req,res){
    if (req.method=='GET'){
        res.end('<form action="/Home/Index" method="POST" name="form1">Enter Text:<input name="txtInput" type="text" id="txtInput"/><button type="submit" id="btnPost">Post Data</button></form></body></html>');
    }else{

        var chunk = '';
        req.on('data', function (data) {
            chunk += data;
        });
        req.on('end', function () {
            console.log(chunk);
            var socket = net.createConnection(50000, "127.0.0.1");
			socket.setEncoding('utf8');
			socket.on("data", function(data){
				socket.write(data);
				});
				socket.write(chunk);
			socket.on("end", function(){
				// can be done else where, is similar to http .end("data", encoding);
				socket.end();
				// cleans up the socket.
				socket.destroy();
			});

            res.end(util.inspect(querystring.parse(chunk)));
        });

    }
}).listen(8000);


