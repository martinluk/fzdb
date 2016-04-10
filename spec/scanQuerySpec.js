var h = require('./support/helper.js');
var gen = require('./support/generator.js');
var _ = require('lodash');

var simpsonsTestData = h.getData("simpsons.fuz");

// load test data

describe("Fuzzy Database", function() { 

  describe("sends the command over TCP", function() {

  beforeAll(function(done) {
    //Not pretty I know, will refactor later once everything is working,.
    h.setupClient();
    h.sendCmd(h.loginToAdminQuery).then(function() {
      h.sendCmd('FLUSH').then(function() {
        h.sendCmd(simpsonsTestData).then(function() {
          h.sendCmd('USER LOGOUT').then(function() {done();});
        });
      });
      });
    });


   // Jonathan: Testing hierarchies.
    h.testCase("Retrieve all entities with the same surname as entity 2", 
      "SELECT $b WHERE { entity:2 <surname> $a . $b <surname> $a}",
      h.resultTemplate(
            [{"b":"3"}, {"b": "2"}]
      ), h.defaultTimeout
    );
 
  });
});
