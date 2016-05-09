var net = require('net');
var fs = require('fs');

var client;
client = new net.Socket();

var sendCmd = function(cmd) {
  client.write(cmd);
  var beforeBytes = client.bytesRead;
  return new Promise(function(resolve, reject) {

    var resultString = "";

    var onDataFunc = function(data) {

      if((client.bytesRead-beforeBytes) < 384) {
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

          if((client.bytesRead-beforeBytes) % 384 != 0) {
            //we are done - disconnect client and resolve promise
            client.removeListener('data', onDataFunc);
            resolve(JSON.parse(resultString));
          }  

          beforeBytes = client.bytesRead;      
      }        
    };

    client.on('data', onDataFunc); 
  });     
};


fs.readFile('liverpool_full.fuz', 'utf8', function (err,dataFile) {
  if (err) {
    return console.log(err);
  }
  client.connect(1407, '127.0.0.1', function() {
	  client.write("FLUSH");
	  client.once('data', function(data) {
	  	sendCmd("USER LOGIN fzydb_admin password").then(function(result) {
		  	sendCmd(dataFile).then(function(result) {
		  		console.log(result);
		  		process.exit();
		  	});
		});
	  });   
	});
});

