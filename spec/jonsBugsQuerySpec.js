var h = require('./support/helper.js');
var _ = require('lodash');

var simpsonsTestData = h.getData("simpsons.fuz");

// load test data

describe("Fuzzy Database", function() { 

  beforeAll(h.setupClient);
  
  describe("sends the command over TCP", function() {

    //tests are run sequentially

    h.testCase("Inserting entities with a subsetOf",  // name
      `INSERT DATA
{
    $a <name> \"England\" .

    $b <name> \"London\";
    <subsetOf> $a
} WHERE {
  NEW($a,Location) .
  NEW($b,Location)
}`, // command
      { status: true, errorCode: 0, info: 'Inserted 3 triples.', result: { type: 'fsparql', data: { a: '2', b: '3' } } } // result
    );

    h.testCase("Retrieving entities entities with a subsetOf",  

      `SELECT $id $other WHERE { $id <subsetOf> $other; <name> "London" }`,

      // result template just adds {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":DATA}} around the given DATA
      h.resultTemplate(
        [ { id: '3', other: '2' }]
      )
    );

    describe("with the simpsons data loaded", function() {

      beforeAll(function(done) {
        h.sendCmd("FLUSH").then(function() {
          h.sendCmd(simpsonsTestData).then(done);
        });
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

      h.testCase("killer query 1",  

        `SELECT $a $b WHERE { entity:2 <surname> $a . entity:3 $b $a . entity:4 <forename> $c }`,

        // result template just adds {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":DATA}} around the given DATA
        h.resultTemplate(
          [{"a":"Simpson","b":"10"}]
        )
      );

    });
 
  });
});
