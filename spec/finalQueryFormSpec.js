var h = require('./support/helper.js');
var _ = require('lodash');

var simpsonsTestData = h.getData("simpsons_meta.fuz");

// load test data

fdescribe("Fuzzy Database:finalQueryFormSpec", function() { 

  
  describe("sends the command over TCP with the simpsons data loaded", function() {

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

      h.testCase("killer query 1",  

        `SELECT $a $b WHERE { entity:2 <surname> $a . entity:3 $b $a . entity:4 <forename> $c }`,

        // result template just adds {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":DATA}} around the given DATA
        h.resultTemplate(
          [{"a":"Simpson","b":"surname"}]
        )
      );

    });
 
});
