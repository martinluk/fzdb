var net = require('net');
var fs = require('fs');

var client;
var helper = {};

helper.sendCmd = function(cmd) {
  client.write(cmd);
  var beforeBytes = client.bytesRead;
  return new Promise(function(resolve, reject) {

    var resultString = "";

    var onDataFunc = function(data) {
      if((client.bytesRead-beforeBytes) < 1024) {
        //we are done - disconnect client and resolve promise
        client.removeListener('data', onDataFunc);
        resolve(JSON.parse(resultString + data));
      } else {
        var dataAsString = data.toString();

        for(var i = 0; i<data.length; i++) {
          if(data[i] != 0) {
            resultString += dataAsString[i];
          }
        }
        
        beforeBytes = client.bytesRead;
      }        
    };

    client.on('data', onDataFunc); 
  });     
};

helper.testCase = function(name, command, expected) {
  it(name, function(done) {
     helper.sendCmd(command)
    .then(function(data) {
      expect(data).toEqual(expected);
      done();
    }); 
  });    
};

helper.resultTemplate = function(results) {
  return {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":results}};
};

helper.setupClient = function(done) {
  client = new net.Socket();
  client.connect(1407, '127.0.0.1', function() {
    client.write("FLUSH");
    client.once('data', function(data) {
      done();
    });   
  });
}

helper.getData = function(name) {
  return fs.readFileSync("./spec/data/" + name, "utf8");
}

module.exports = helper;
