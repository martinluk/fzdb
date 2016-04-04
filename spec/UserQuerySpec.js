var net = require('net');

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

  var assertNotEnoughPermission= function(q, done){
    sendCmd(q).then(function(data) {
        expect(data.status).toBe(false);
        expect(data.info).toBe('Insufficient permission to run the job');
        done();
    });    
  }
    var admin_username = 'fzydb_admin';
    var admin_pwd = 'password';
    var space = ' ';
  var sampleQuery = {
      'insert'         : "INSERT DATA { $a <forename> \"Fred\" } WHERE { NEW($a,\"person\") }",
      'flush'          : "FLUSH",
      'logout'         : "USER LOGOUT",
      'user_add'       : "USER ADD creativeUserName verybadpassword",
      'user_login'     : "USER LOGIN creativeUserName verybadpassword",
      "user_delete"    : "USER DELETE creativeUserName",
      "user_password"  : "USER PASSWORD verybadpassword betterpassword",
      "user_promote"   : "USER PROMOTE creativeUserName",
      "user_demote"    : "USER DEMOTE creativeUserName",
      "login_to_admin" : "USER LOGIN"+space+admin_username+space+admin_pwd
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
    
    executeEditorFunction=function(name, command) {
        var login={name:'editorAcc', password:'password'};
        describe(name, function() {
            it("Log into Admin", function(done) {
                sendCmd(sampleQuery.login_to_admin).then(function(data) { done(); });
            });
            it("Add User", function(done) {
                addQ='USER ADD '+login.name+' '+login.password;
                sendCmd(addQ).then(function(data) { done(); });
            });
            it("Logout from Admin", function(done) {
                sendCmd('USER LOGOUT').then(function(data) { done(); });
            });
            it("Login to user", function(done) {
                q='USER LOGIN '+login.name+' '+login.password;
                sendCmd(q).then(function(data) { done(); });
            });
            it(name, function(done) {
                sendCmd(command).then(function(data) { done(); });
            });
            it("Logout from user", function(done) {
                sendCmd('USER LOGOUT').then(function(data) { done(); });
            });
            it("Log into Admin", function(done) {
                sendCmd(sampleQuery.login_to_admin).then(function(data) { done(); });
            });
            it("Delete the user", function(done) {
                dQ='USER DELETE '+login.name;
                sendCmd(dQ).then(function(data) { done(); });
            });
            it("Logout from Admin", function(done) {
                sendCmd('USER LOGOUT').then(function(data) { done(); });
            });
        });
    }

    describe("editor not allowed to", function() {
        executeEditorFunction('flush command', 'FLUSH');
        executeEditorFunction('promote command', sampleQuery.user_promote);
        executeEditorFunction('demote command', sampleQuery.user_demote);

    });
  });
});
