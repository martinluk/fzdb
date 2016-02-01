var net = require('net');

describe("Fuzzy Database", function() {
  var client;

  //connects to the database
  beforeEach(function(done) { 
    client = new net.Socket();
    client.connect(1407, '127.0.0.1', function() {
      done();
    });
  });

  
  describe("sends the command over TCP", function() {

    //tests are run sequentially

    //test pong
    it("'PONG'", function(done) {
      client.write("PING");
      client.on('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON.response).toBe("PONG");
        done();
      });      
    });

    //test insert
    it("setting entity:1's forname to Fred", function(done) {
      client.write("INSERT DATA { entity:1 <forename> \"Fred\" }");
      client.on('data', function(data) {
        done();
      });      
    });

    //test retrieve
    it("getting the forename of entity:1", function(done) {
      client.write("SELECT $a WHERE { entity:1 <forename> $a }");
      client.on('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON.result[0].a).toBe("Fred");
        done();
      });      
    });

    it("getting entities with forename 'Fred'", function(done) {
      client.write("SELECT $a WHERE { $a <forename> \"Fred\" }");
      client.on('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON.result[0].a).toBe("1");
        done();
      });      
    });

  });
});