var h = require('./support/helper.js');
var _ = require('lodash');

var simpsonsTestData = h.getData("simpsons.fuz");

// load test data

describe("Fuzzy Database", function() { 

  beforeAll(h.setupClient);
  
  describe("sends the command over TCP", function() {

    //tests are run sequentially

    h.testCase("Adding 150 entities",  // name
      `INSERT DATA
{
    entity:1 <type> \"Location\";
    <name> \"England\" .

    entity:2 <type> \"Location\";
    <name> \"London\";
    <subsetOf> entity:1
}`, // command
      { status: true, errorCode: 0, info: '', result: { type: 'text', data: 'Inserted 5 triples.' } } // result
    );

    h.testCase("Retrieving entities which have a profession set - 50 entities",  

      `SELECT $id $other WHERE { $id <subsetOf> $other; <name> "London" }`,

      // result template just adds {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":DATA}} around the given DATA
      h.resultTemplate(
        [ { id: '2', other: '1' }]
      )
    );

    describe("with the simpsons data loaded", function() {

      beforeAll(function(done) {
        h.sendCmd(simpsonsTestData).then(done);
      });

      h.testCase("retrieving all forenames and surnames",  

      `SELECT $forename $surname WHERE { $id <forename> $forename ; <surname> $surname }`,

      // result template just adds {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":DATA}} around the given DATA
      h.resultTemplate(
        [{"forename": "Homer", "surname": "Simpson"},
        {"forename": "Homer", "surname": "Power"}, 
        {"forename": "Max", "surname": "Simpson"}, 
        {"forename": "Max", "surname": "Power"}, 
        {"forename": "Marge", "surname": "Simpson"},
        {"forename": "Marge", "surname": "Bouvier"}, 
        {"forename": "Ned", "surname": "Flanders"}, 
        {"forename": "Moe", "surname": "Szyslak"}]
      )
    );

    });
 
  });
});
