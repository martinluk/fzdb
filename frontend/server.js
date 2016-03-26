var express = require('express');
var bodyParser = require('body-parser');
var net = require('net');

var app = express(); 

// create application/json parser
var jsonParser = bodyParser.json()

app.use("/", express.static(__dirname + '/public'));

// POST method route
app.post('/', jsonParser, function (req, res) {
  client = new net.Socket();
  console.log(req.body);  
  if(req.body.query) {
    client.connect(1407, '127.0.0.1', function() {
      client.write(unescape(req.body.query));
      client.once('data', function(data) {
        res.send(data);
      });   
    }); 
  } else {
    console.log("No query :(");
  }
});

app.post('/connect', jsonParser, function (req, res) {
  client = new net.Socket();
  console.log(req.body);  
  if(req.body.query) {
    client.connect(1407, '127.0.0.1', function() {
      client.write(unescape(req.body.query));
      client.once('data', function(data) {
        res.send(data);
      });   
    }); 
  } else {
    console.log("No query :(");
  }
});

app.post('/disconnect', jsonParser, function (req, res) {
  client = new net.Socket();
  console.log(req.body);  
  if(req.body.query) {
    client.connect(1407, '127.0.0.1', function() {
      client.write(unescape(req.body.query));
      client.once('data', function(data) {
        res.send(data);
      });   
    }); 
  } else {
    console.log("No query :(");
  }
});

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
});