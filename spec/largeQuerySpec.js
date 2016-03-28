var h = require('./support/helper.js');
var gen = require('./support/generator.js');
var _ = require('lodash');

// load test data
var large_150 = h.getData("large_150.fuz");

describe("Fuzzy Database", function() { 

  beforeAll(h.setupClient);
  
  describe("sends the command over TCP", function() {

    //tests are run sequentially

    h.testCase("Adding 150 entities",  // name
      large_150, // command
      { status: true, errorCode: 0, info: '', result: { type: 'text', data: 'Inserted 350 triples.' } } // result
    );

    h.testCase("Retrieving entities which have a profession set - 50 entities",  

      "SELECT $a WHERE { $a <profession> $b}",

      // result template just adds {"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":DATA}} around the given DATA
      h.resultTemplate(

        // generate results using lodash - a very very useful js library for pretty much everything
        _.range(101,151).map(function(id) { return { a: id.toString() }; })
      )
    );

    h.testCase("Retrieving entities which have the forename set - 150 entities",  

      "SELECT $a WHERE { $a <forename> $b}",

      h.resultTemplate(
       _.range(1,151).map(function(id) { return { a: id.toString() }; })
      )
    );
 
  });
});
