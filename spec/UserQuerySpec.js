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
  
  var assertNotEnoughPermission= function(q, done){
    sendCmd(q).then(function(data) {
        expect(data.status).toBe(false);
        expect(data.info).toBe('Insufficient permission to run the job');
        done();
    });    
  }

  var sampleQuery = {
      'insert'        : "INSERT DATA { $a <forename> \"Fred\" } WHERE { NEW($a,\"person\") }",
      'flush'         : "FLUSH",
      'logout'        : "USER LOGOUT",
      'user_add'      : "USER ADD creativeUserName verybadpassword",
      "user_delete"   : "USER DELETE creativeUserName",
      "user_password" : "USER PASSWORD verybadpassword betterpassword",
      "user_promote"  : "USER PROMOTE creativeUserName",
      "user_demote"   : "USER DEMOTE creativeUserName"
  };

  fdescribe("User Queries:", function() {
    describe("Guests not allowed to", function() {
        it("run insert query", function(done) {
            assertNotEnoughPermission(sampleQuery.insert,done);
        });
        it("run flush query", function(done) {
            assertNotEnoughPermission(sampleQuery.flush,done);
        });
        it("run logout query", function(done) { 
            assertNotEnoughPermission(sampleQuery.logout,done);
        });
        it("run user add query", function(done) { 
            assertNotEnoughPermission(sampleQuery.user_add,done);
        });
        it("run change password query", function(done) { 
            assertNotEnoughPermission(sampleQuery.user_password,done);
        });
        it("run user promote query", function(done) { 
            assertNotEnoughPermission(sampleQuery.user_promote,done);
        });
        it("run user demote query", function(done) { 
            assertNotEnoughPermission(sampleQuery.user_demote,done);
        });
        //TODO Load query
    });
  });
});
