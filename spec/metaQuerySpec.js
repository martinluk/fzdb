var net = require('net');
var fs = require('fs');

var simpsonsTestData = fs.readFileSync("./spec/data/simpsons.fuz", "utf8");

describe("Fuzzy Database", function() {
  var client;

  var sendCmd = function(cmd) {
    client.write(cmd);
    return new Promise(function(resolve, reject) {
      client.once('data', function(data) {
        resolve(JSON.parse(data));
      }); 
    });     
  }

  //connects to the database
  beforeAll(function(done) { 
    client = new net.Socket();
    client.connect(1407, '127.0.0.1', function() {
      client.write("FLUSH");
      client.once('data', function(data) {
          done();  
      });   
    });
  });


  
  describe("sends the command over TCP", function() {

    //tests are run sequentially

   //test insert
    it("simple meta test", function(done) {
      sendCmd(simpsonsTestData).then(function() {
         sendCmd('SELECT $a WHERE { META $b { $a <forename> "Max" } . $b <testProp> "test" }')
        .then(function(data) {
          done();
        });    
      });
     
    });    

  });
});