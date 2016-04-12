var h = require('./support/helper.js');
var gen = require('./support/generator.js');
var _ = require('lodash');

// load test data

describe("Fuzzy Database: Missing Values", function() { 

  
  describe("sends the command over TCP", function() {

  beforeAll(function(done) {
    //Not pretty I know, will refactor later once everything is working,.
    h.setupClient();
    h.sendCmd(h.loginToAdminQuery).then(function() {
      h.sendCmd('FLUSH').then(function() {
          h.sendCmd('USER LOGOUT').then(function() {done();});
        });
      });
    });


    //tests are run sequentially

    h.testCase("Selecting non-existent property", 
      "SELECT $a WHERE { entity:1 <doesnotexist> $a }",
      {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":[]}}
    );
 
  });
});
