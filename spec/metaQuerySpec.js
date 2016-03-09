var net = require('net');
var fs = require('fs');

var simpsonsTestData = fs.readFileSync("./spec/data/simpsons_meta.fuz", "utf8");

describe("Fuzzy Database", function() {
  var client;

  var sendCmd = function(cmd) {
    client.write(cmd);
    return new Promise(function(resolve, reject) {
      client.once('data', function(data) {
        resolve(JSON.parse(data));
      }); 
    });     
  };

  var testCase = function(name, command, expected) {
    it(name, function(done) {
       sendCmd(command)
      .then(function(data) {
        expect(data).toEqual(expected);
        done();
      }); 
    });    
  };

  var resultTemplate = function(results) {
    return {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":results}};
  };

  //connects to the database
  beforeAll(function(done) { 
    client = new net.Socket();
    client.connect(1407, '127.0.0.1', function() {      
     sendCmd("FLUSH").then(function() {
      sendCmd(simpsonsTestData).then(function() {
        done();
      });
     });
    });
  });
  
  describe("has meta data functionality : ", function() {

    //simple check of basic meta functionality
    describe("simple test : ", function() {

      testCase("sanity check", 'SELECT $a WHERE { $a <forename> "Homer" }',
        resultTemplate([
          {"a":"1"}
        ]));

      testCase("no restrictions on meta value", 'SELECT $a WHERE { META $b { $a <forename> "Homer" } }',
        resultTemplate([
          {"a":"1"}
        ]));

      testCase("meta value restricted and false", 'SELECT $a WHERE { META $b { $a <forename> "Homer" } . $b <test> "Kit" }',
        resultTemplate([]));

      testCase("meta value restricted and true", 'SELECT $a WHERE { META $b { $a <forename> "Homer" } . $b <test> "Cake" }',
        resultTemplate([
          {"a":"1"}
        ]));
    });

    describe("retrieving meta values : ", function() {

    });

  });
});