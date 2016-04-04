var net = require('net');

describe("Fuzzy Database", function() {
  var client;

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

  var sendCmd = function(cmd) {
    client.write(cmd);
    return new Promise(function(resolve, reject) {
      client.once('data', function(data) {
        resolve(JSON.parse(data));
      }); 
    });     
  }
  
  var assertNotEnoughPermission= function(query, done){
    sendCmd(q).then(function(data) {
        expect(data.status).toBe(false);
        expect(data.info).toBe('Insufficient permission to run the job');
        done();
    });    
  }

  var sampleQuery = {
      'insert':"INSERT DATA { $a <forename> \"Fred\" } WHERE { NEW($a,\"person\") }"
  };

  fdescribe("User Queries:", function() {
    describe("Guests now allowed to", function() {
        it("run insert query", function(done) {
            q=sampleQuery.insert;
            assertNotEnoughPermission(q,done);
    });



    });
  });
});
