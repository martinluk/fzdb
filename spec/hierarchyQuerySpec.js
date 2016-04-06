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
      "INSERT DATA { $eng <name> \"England\" . $lon <name> \"London\"; <subsetOf> $eng . $eur <name> \"Europe\"; <supersetOf> $eng } WHERE { NEW($eng, \"Location\") . NEW($lon, \"Location\") . NEW($eur, \"Location\") }",
      {"status":true,"errorCode":0,"info":"Inserted 5 triples.","result":{"type":"fsparql","data":{"eng":"2","eur":"3", "lon":"4"}}}
    );

    h.testCase("Querying the hierarchy for superset link", 
      "SELECT $a WHERE { $a <supersetOf> $b }",
      h.resultTemplate(
            [{"a":"2"}, {"a":"3"}]
      )
    );

    h.testCase("Querying the hierarchy for subset link", 
      "SELECT $a WHERE { $a <subsetOf> $b }",
      h.resultTemplate(
            [{"a":"2"}, {"a":"4"}]
      )
    );

    h.testCase("Inserting an entity with hierarchy data", 
      "INSERT DATA { $per <name> \"Fred\" ; <lives> entity:4 } WHERE { NEW($per, \"Person\") }",
      {"status":true,"errorCode":0,"info":"Inserted 2 triples.","result":{"type":"fsparql","data":{"per": "5"}}}
    );

    h.testCase("Getting entities with exact match", 
      "SELECT $a WHERE { $a <lives> entity:4 }",
      h.resultTemplate(
            [{"a":"5"}]
      )
    );

    h.testCase("Getting entities with superset match", 
      "SELECT $a WHERE { $a <lives> entity:2 }",
      h.resultTemplate(
            [{"a":"5"}]
      )
    );

    h.testCase("Getting entities with superset match", 
      "SELECT $a WHERE { $a <lives> entity:3 }",
      h.resultTemplate(
            [{"a":"5"}]
      )
    );
 
  });
});
