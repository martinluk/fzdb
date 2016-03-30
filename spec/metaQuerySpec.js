var h = require('./support/helper.js');

var simpsonsTestData = h.getData("simpsons_meta.fuz");

describe("Fuzzy Database", function() { 

  beforeAll(h.setupClient);
  
  describe("with simpsons data loaded has meta data functionality : ", function() {

    beforeAll(function(done) {
      h.sendCmd(simpsonsTestData).then(done);
    });

    //simple check of basic meta functionality
    describe("simple test : ", function() {

      h.testCase("sanity check", 'SELECT $a WHERE { $a <forename> "Homer" }',
        h.resultTemplate([
          {"a":"2"}
        ]));

      h.testCase("no restrictions on meta value", 'SELECT $a WHERE { META $b { $a <forename> "Homer" } }',
        h.resultTemplate([
          {"a":"2"}
        ]));

      h.testCase("meta value restricted and false", 'SELECT $a WHERE { META $b { $a <forename> "Homer" } . $b <test> "Kit" }',
        h.resultTemplate([]));

      h.testCase("meta value restricted and true", 'SELECT $a WHERE { META $b { $a <forename> "Homer" } . $b <test> "Cake" }',
        h.resultTemplate([
          {"a":"2"}
        ]));
    });

    describe("retrieving meta values : ", function() {

    });

  });
});