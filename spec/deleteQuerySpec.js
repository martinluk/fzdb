var net = require('net');
var Promise = require("es6-promise").Promise

describe("fzdb", function() {
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

	describe("delete queries:", function() {
		beforeEach(function(done){

		});
		describe("DB with one entity", function() {
			it("no one used to call Fred", function(done) {
				sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }").then(function(data) {
					expect((data.result.data).length).toBe(0); done();
				});          
			});
			it("Adding Fred into DB", function(done) {
				sendCmd("INSERT DATA { entity:1 <forename> \"Fred\" }").then(function(data) {
					done(); 
				});    
			});
			it("and now Fred lives in DB", function(done) {
				sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }").then(function(data) {
					expect((data.result.data).length).toBe(1);
					done();
				});          
			});
			it("Running the delete query", function(done) {
				sendCmd("DELETE DATA { entity:1 <forename> \"Fred\" }").then(function(data) {
					done();
				});    
			});
			it("Resulting in Fred no longer lives in DB", function(done) {
				sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }").then(function(data) {
					expect((data.result.data).length).toBe(0);
					done();
				});          
			});
		});
	});
});
