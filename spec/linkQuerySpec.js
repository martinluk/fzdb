var net = require('net');
var Promise = require('promise');

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
  
  describe("sends the command over TCP", function() {

    //tests are run sequentially

    //test insert
    it("setting entity:1's forename to Fred", function(done) {
      sendCmd("INSERT DATA { entity:1 <forename> \"Fred\" }")
      .then(function(data) {
        done();
      });    
    });

    //test insert
    it("setting entity:2's surname to Smith", function(done) {
      sendCmd("INSERT DATA { entity:2 <surname> \"Smith\" }")
      .then(function(data) {
        done();
      });       
    });

    //test retrieve
    it("getting the forename of entity:1", function(done) {
      sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }")
      .then(function(data) {
        expect(data.result[0].a).toBe("1");
        done();
      });          
    });

    //test retrieve
    it("getting the surname of entity:2", function(done) {  
      sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }")
      .then(function(data) {
        expect(data.result[0].a).toBe("2");
        done();
      });
    });

    //test link
    it("link entity 1 and entity 2", function(done) {
      sendCmd("LINK entity:1 entity:2")
      .then(function(data) {
        expect(data.result).toBe("success");
        done();
      });     
    });

    //test retrieve
    it("getting the forename of entity:1 after link", function(done) {
      sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }")
      .then(function(data) {
        expect(data.result[0].a).toBe("1");
        done();
      });      
    });

    //test retrieve
    it("getting the surname of entity:2 after link", function(done) {
      sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }")
      .then(function(data) {
        expect(data.result[0].a).toBe("1");
        done();
      });      
    });

    //test link
    it("unlink entity 1 and entity 2", function(done) {
      sendCmd("UNLINK entity:1 entity:2")
      .then(function(data) {
        expect(data.result).toBe("success");
        done();
      });     
    });

    //test retrieve
    it("getting the forename of entity:1 after unlink", function(done) {
      sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }")
      .then(function(data) {
        expect(data.result[0].a).toBe("1");
        done();
      });          
    });

    //test retrieve
    it("getting the surname of entity:2 after unlink", function(done) {  
      sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }")
      .then(function(data) {
        expect(data.result[0].a).toBe("2");
        done();
      });
    });

  });
});