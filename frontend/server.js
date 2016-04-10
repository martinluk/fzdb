var express = require('express');
var bodyParser = require('body-parser');
var net = require('net');
var session = require('express-session')
var uuid = require('node-uuid');

var app = express(); 

// create application/json parser
var jsonParser = bodyParser.json()

var clients = {};

app.use(session({ secret: 'keyboard cat', cookie: { maxAge: 60000 }, resave: false, saveUninitialized: false}));

app.use("/", express.static(__dirname + '/public'));

// POST method route
app.post('/', jsonParser, function (req, res) {
  console.log(req.body);  
  if(req.body.query) {  
      var client = clients[req.session.conn];
      client.write(unescape(req.body.query));
      client.once('data', function(data) {
        res.send(data);
      });   
  } else {
    console.log("No query :(");
  }
});

app.post('/connect', jsonParser, function (req, res) {  
  var client = new net.Socket();
  client.connect(1407, '127.0.0.1', function() {
    console.log("connected!");
    var id = uuid.v4();
    clients[id] = client;
    req.session.conn = id;
    res.send("success");
    console.log("CONNECTED!!", req.session);
  });
});

app.post('/disconnect', jsonParser, function (req, res) {
  var client = clients[req.session.conn];
  client.destroy();
  clients[req.session.conn] = undefined;
  req.session.destroy(function(err) {
    res.send("success");
  })
});

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
});