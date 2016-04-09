var h = require('./support/helper.js');
var _ = require('lodash');

var simpsonsTestData = h.getData("simpsons.fuz");

// load test data

describe("Fuzzy Database", function() { 

  
  describe("sends the command over TCP", function() {

    //tests are run sequentially

    describe("with the simpsons data loaded", function() {


    beforeAll(function(done) {
      //Not pretty I know, will refactor later once everything is working,.
      h.setupClient();
      h.sendCmd(h.loginToAdminQuery).then(function() {
        h.sendCmd('FLUSH').then(function() {
          h.sendCmd(simpsonsTestData).then(function(){
            h.sendCmd('USER LOGOUT').then(function() {done();});
          });
        });
        });
      });


      h.testCase("retrieving all entities with the same forename and surname",  

        "SELECT $a WHERE { $a <forename> $b ; <surname> $b }",

        // result template just adds {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":DATA}} around the given DATA
        h.resultTemplate(
          []
        )
      );

      h.testCase("adding surname 'Homer' to 'Homer'",  

        `INSERT DATA { $a <surname> "Homer" } WHERE { $a <forename> "Homer" }`,

        // result template just adds {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":DATA}} around the given DATA
        function(data, done) {
          done();
        }
      );

      //Skipping, since is an assertion to above skipped test case.
      h.testCase("retrieving all entities with the same forename and surname 2",  

        "SELECT $a WHERE { $a <forename> $b ; <surname> $b }",

        // result template just adds {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":DATA}} around the given DATA
        h.resultTemplate(
          [{"a":"2"}]
        )
      );

    });
 
  });
});
