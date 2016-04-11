var net = require('net');
var h = require('./support/helper.js');


describe("Fuzzy Database", function() {
  var client;

  //connects to the database
  beforeAll(function(done) { 
    client = new net.Socket();
    client.connect(1407, '127.0.0.1', function() {
        client.write(h.loginToAdminQuery);
        client.once('data', function(data) {
            var resultJSON = JSON.parse(data);
            expect(resultJSON.result.data).toEqual('Logged in successfully.');
            client.write("FLUSH");
            client.once('data', function(data) {
                var resultJSON = JSON.parse(data);
                client.write('USER LOGOUT');
                client.once('data', function(data) {
                    var resultJSON = JSON.parse(data);
                    done();
                });
            });   
        });
      });
    });
  
  describe("sends the command over TCP", function() {

    it("Sample query - logging into admin", function(done) {
      client.write(h.loginToAdminQuery);
      client.once('data', function(data) {
            var resultJSON = JSON.parse(data);
            expect(resultJSON.result.data).toEqual('Logged in successfully.');
            done();
        });
    });

	it("Adding a date", function(done) {
      client.write("INSERT DATA { $a <date> 25/12/2009 } WHERE { NEW($a, \"Present\") }");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({"status":true,"errorCode":0,"info":"Inserted 1 triples.","result":{"type":"fsparql","data":{"a":"2"}}}));
        done();
      });      
    });

	it("Querying the date", function(done) {
      client.write("SELECT $d WHERE { $e <date> $d }");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":[{"d":"25/12/2009"}]}}));
        done();
      });      
    });

	it("Sample query - logging out from admin", function(done) {
        client.write('USER LOGOUT');
        client.once('data', function(data) {
            var resultJSON = JSON.parse(data);

        done();
        });
    });
 });   
}); 
