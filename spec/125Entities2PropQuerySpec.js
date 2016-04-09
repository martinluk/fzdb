var h = require('./support/helper.js');
var gen = require('./support/generator.js');

var largeData = gen([{
  count: 125,
  type: "person",
  template: `<forename> "Alexis#"; <surname> "Sanchez#"`
}]);

describe("Fuzzy Database 125spec", function() {
  var client;

  //connects to the database
  beforeAll(
      function(done) {
          h.setupClient();
          done()
      });

  h.testCase("NANA", "FLUSH",
    {status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})});

  h.testCase("Adding 125 entities", largeData,
    function(data, done) {
      done();
    }, 10000);

  h.testCase("NANA", "FLUSH",
    {status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})});	
	  
}); 
