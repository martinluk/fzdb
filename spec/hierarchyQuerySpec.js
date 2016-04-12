var h = require('./support/helper.js');
var gen = require('./support/generator.js');
var _ = require('lodash');

// load test data

describe("Fuzzy Database: Hierarchy", function() { 

  
  describe("sends the command over TCP", function() {

  beforeAll(function(done) {
    //Not pretty I know, will refactor later once everything is working,.
    h.setupClient();
    h.sendCmd(h.loginToAdminQuery).then(function() {
      h.sendCmd('FLUSH').then(function() {
          done();
        });
      });
    });

  afterAll(function(done) {
    h.sendCmd('USER LOGOUT').then(function() {done();});
  });


    //tests are run sequentially

   // Jonathan: Testing hierarchies.
    h.testCase("Creating a simple hierarchy", 
      "INSERT DATA { $eng <name> \"England\" . $fra <name> \"France\" . $lon <name> \"London\"; <subsetOf> $eng . $bir <name> \"Birmingham\"; <subsetOf> $eng . $eur <name> \"Europe\"; <supersetOf> $eng , $fra } WHERE { NEW($eng, \"Location\") . NEW($lon, \"Location\") . NEW($eur, \"Location\") .  NEW($fra, \"Location\") . NEW($bir, \"Location\") }",
      {"status":true,"errorCode":0,"info":"Inserted 9 triples.","result":{"type":"fsparql","data":{"bir": "2", "eng":"3","eur":"4", "fra": "5", "lon":"6"}}}
    );

    h.testCase("Querying the hierarchy for superset link", 
      "SELECT $a WHERE { $a <supersetOf> $b }",
      h.resultTemplate(
            [{"a":"3"}, {"a":"3"}, {"a":"4"}, {"a":"4"}]
      )
    );

    h.testCase("Querying the hierarchy for subset link", 
      "SELECT $a WHERE { $a <subsetOf> $b }",
      h.resultTemplate(
            [{"a":"2"}, {"a":"3"}, {"a":"5"}, {"a":"6"}]
      )
    );

    h.testCase("Inserting an entity with hierarchy data", 
      "INSERT DATA { $per <name> \"George\" ; <lives> entity:4 } WHERE { NEW($per, \"Person\") }",
      {"status":true,"errorCode":0,"info":"Inserted 2 triples.","result":{"type":"fsparql","data":{"per": "7"}}}
    );

    h.testCase("Inserting an entity with hierarchy data", 
      "INSERT DATA { $per <name> \"Fred\" ; <lives> entity:6 } WHERE { NEW($per, \"Person\") }",
      {"status":true,"errorCode":0,"info":"Inserted 2 triples.","result":{"type":"fsparql","data":{"per": "8"}}}
    );

    h.testCase("Getting entities with superset match 1", 
      "SELECT $a WHERE { $a <lives> entity:2 }",
      h.resultTemplate(
            [{"a":"7"}]
      )
    );

    h.testCase("Getting entities with superset match 2", 
      "SELECT $a WHERE { $a <lives> entity:3 }",
      h.resultTemplate(
            [{"a":"8"}, {"a":"7"}]
      )
    );

    h.testCase("Getting entities with superset match 3", 
      "SELECT $a WHERE { $a <lives> entity:4 }",
      h.resultTemplate(
            [{"a":"7"}, {"a":"8"}]
      )
    );

     h.testCase("Getting entities with subset match 4", 
      "SELECT $a WHERE { $a <lives> entity:5 }",
      h.resultTemplate(
            [{"a":"7"}]
      )
    );

     h.testCase("Getting entities with subset match 5", 
      "SELECT $a WHERE { $a <lives> entity:6 }",
      h.resultTemplate(
            [{"a":"8"}, {"a":"7"}]
      )
    );
 
  });
});
