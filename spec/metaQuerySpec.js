var h = require('./support/helper.js');

var simpsonsTestData = h.getData("simpsons_meta.fuz");

describe("Fuzzy Database:MetadataSpec", function() { 

  
  describe("with simpsons data loaded has meta data functionality : ", function() {


  beforeAll(function(done) {
    h.setupClient();
    h.sendCmd(h.loginToAdminQuery).then(function() {
      h.sendCmd('FLUSH').then(function() {
        h.sendCmd(simpsonsTestData).then(function() {
          h.sendCmd('USER LOGOUT').then(function() {done();});
        });
      });
    });
  });

    //simple check of basic meta functionality
    describe("simple test : ", function() {

      h.testCase("sanity check", 'SELECT $a WHERE { $a <forename> "Homer" }',
        h.resultTemplate([
          {"a":"2"}
        ]));

	  h.testCase("sanity check", 'SELECT $a $b WHERE { $a <forename> $b }',
        h.resultTemplate([
          {a: '2', b:'Homer'}, {a: '3', b:'Marge'}, {a: '4', b:'Ned'}, {a: '5', b:'Moe'}, {a:'2', b:'Max'}       
   	    ]));
		
	  h.testCase("sanity check", 'SELECT $a $b WHERE { $a <surname> $b }',
        h.resultTemplate([
          {a: '2', b:'Simpson'}, {a: '3', b:'Simpson'}, {a: '4', b:'Flanders'}, {a: '5', b:'Szyslak'}, {a:'2', b:'Power'}, {a:'3', b:'Bouvier'}       
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

    describe("retrieving meta values", function() {
      h.testCase("source",
       'SELECT $a $c WHERE { META $b { entity:2 <forename> $c } . $b <fuz:source> $a }',
        h.resultTemplate([
          {"a":"0","c":"Homer"},{"a":"0","c":"Max"}
        ]));
    });

  });
});
