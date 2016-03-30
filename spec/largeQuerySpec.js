var h = require('./support/helper.js');
var gen = require('./support/generator.js');
var _ = require('lodash');

// load test data
var large_150 = gen([{
  count: 100,
  type: "person",
  template: `<forename> "Alexis#"; <surname> "Sanchez#"`
}, {
  count: 50,
  type: "person",
  template: `<forename> "Alexis#"; <surname> "Sanchez#"; <profession> "Bartender"`
}]);

describe("Fuzzy Database", function() { 

  beforeAll(h.setupClient);
  
  describe("sends the command over TCP", function() {

    //tests are run sequentially

    var entityAssignments = {};

    h.testCase("Adding 150 entities",  // name
      large_150, // command
      function(data, done) { entityAssignments = data.result.data; expect(data.info).toEqual('Inserted 350 triples.'); done(); }, 20000);

    h.testCase("Retrieving entities which have a profession set - 50 entities",  

      "SELECT $a WHERE { $a <profession> $b}",

      function(data, done) {
        var res = h.resultTemplate(
          // generate results using lodash - a very very useful js library for pretty much everything
          _.range(101,151).map(function(id) { return { a: entityAssignments["per" + id].toString() }; })
        );

        expect(data).toEqual(res);
        done();
     }
    );

    h.testCase("Retrieving entities which have the forename set - 150 entities",  

      "SELECT $a WHERE { $a <forename> $b}",

      function(data, done) {
        
        var res = h.resultTemplate(
          // generate results using lodash - a very very useful js library for pretty much everything
          _.range(1,151).map(function(id) { return { a: entityAssignments["per" + id].toString() }; })
        );

        //results come back in sorted order, sort them!
        res.result.data.sort(function(a,b) { return parseInt(a.a) > parseInt(b.a) ? 1 : -1; });

        expect(data).toEqual(res);
        done();
     }, 20000
    );
 
  });
});
