var net = require('net');
var Promise = require("es6-promise").Promise

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
    var fredId;
    it("setting entity:1's forename to Fred", function(done) {
      sendCmd("INSERT DATA { $a <forename> \"Fred\" } WHERE { NEW($a,person) }")
      .then(function(data) {
        fredId = data.result.data.a;
        done();
      });    
    });

    //test insert
    var smithId;
    it("setting entity:2's surname to Smith", function(done) {
      sendCmd("INSERT DATA { $a <surname> \"Smith\" } WHERE { NEW($a,person) }")
      .then(function(data) {
        smithId = data.result.data.a;
        done();
      });       
    });

    //test retrieve
    it("getting the forename of entity:1", function(done) {
      sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }")
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: fredId})]})}));
        done();
      });          
    });

    //test retrieve
    it("getting the surname of entity:2", function(done) {  
      sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }")
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: smithId})]})}));
        done();
      });
    });

    //test link
    it("merge entity 1 and entity 2", function(done) {
      sendCmd("LINK FINAL entity:" + fredId + " entity:" + smithId)
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Entities ' + fredId + ' and ' + smithId + ' merged successfully.'})}));
        done();
      });     
    });

    //test retrieve
    it("getting the forename of entity:1 after merge", function(done) {
      sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }")
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: fredId})]})}));
        done();
      });      
    });

    //test retrieve
    it("getting the surname of entity:2 after merge", function(done) {
      sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }")
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: fredId})]})}));
        done();
      });      
    });

    //test link
    it("getting properties of entity:2", function(done) {
      sendCmd("SELECT $a $b WHERE { entity:" + smithId + " $a $b }")
      .then(function(data) {
        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[]})}));
        done();
      });     
    });   

  });
});
