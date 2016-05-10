var net = require('net');
var stdin = process.openStdin();

var client = new net.Socket();
client.connect(1407, '127.0.0.1', function() {
	console.log('Connected');
	process.stdout.write("> ")
	stdin.addListener("data", function(d) {
		client.write(d.toString().trim());
	});
});

client.on('data', function(data) {
	console.log("");
	console.log(data.toString());
	console.log("");
	process.stdout.write("> ")
});

client.on('close', function() {
	console.log('Connection closed');
});

client.on('error', function(e) {
	console.log("\n\r" + e.toString());
	process.exit();
});