var h = require('./support/helper.js');
var gen = require('./support/generator.js');
var _ = require('lodash');

// load test data

describe("Fuzzy Database", function() { 

  beforeAll(h.setupClient);
  
  describe("sends the command over TCP", function() {

    //tests are run sequentially

   // Jonathan: Testing hierarchies.
    h.testCase("Creating a simple hierarchy", 
      "INSERT DATA { $eng <name> \"England\" . $lon <name> \"London\"; <subsetOf> $eng } WHERE { NEW($eng, \"Location\") . NEW($lon, \"Location\") }",
      {"status":true,"errorCode":0,"info":"Inserted 3 triples.","result":{"type":"fsparql","data":{"eng":"2","lon":"3"}}}
    );

    h.testCase("Querying the hierarchy for superset link", 
      "SELECT $a WHERE { $a <supersetOf> $b }",
      h.resultTemplate(
            // generate results using lodash - a very very useful js library for pretty much everything
            [{"a":"2"}]
      )
    );

    h.testCase("Querying the hierarchy for subset link", 
      "SELECT $a WHERE { $a <subsetOf> $b }",
      h.resultTemplate(
            // generate results using lodash - a very very useful js library for pretty much everything
            [{"a":"3"}]
      )
    );
 
  });
});
