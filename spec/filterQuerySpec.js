var net = require('net');
var h = require('./support/helper.js');


describe("Fuzzy Database", function() {
  var client;

  //connects to the database
  beforeAll(function(done) { 
    client = new net.Socket();
    client.connect(1407, '127.0.0.1', function() {
        client.write(h.loginToAdminQuery);
        client.once('data', function(data) {
            var resultJSON = JSON.parse(data);
            expect(resultJSON.result.data).toEqual('Logged in successfully.');
            client.write("FLUSH");
            client.once('data', function(data) {
                var resultJSON = JSON.parse(data);
                client.write('USER LOGOUT');
                client.once('data', function(data) {
                    var resultJSON = JSON.parse(data);
                    done();
                });
            });   
        });
      });
    });
  
  describe("sends the command over TCP", function() {

    it("Sample query - logging into admin", function(done) {
      client.write(h.loginToAdminQuery);
      client.once('data', function(data) {
            var resultJSON = JSON.parse(data);
            expect(resultJSON.result.data).toEqual('Logged in successfully.');
            done();
        });
    });

	//discard current data
	it("flush everything", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });
	
	it("setting entity:2's forename to 'Marco'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Marco\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	it("setting entity:3's forename to 'Marc'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Marc\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	it("setting entity:4's forename to 'Arco'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Arco\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	it("setting entity:5's forename to 'Mar'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Mar\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	it("Using the regex filter", function(done) {
      client.write("SELECT $a $b WHERE { $a <forename> $b . FILTER regex($b, ^rc)}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":[]}}));
        done();
      });      
    });
	
	//discard current data
	it("flush everything", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });
	
    it("setting entity:2's forename to 'Marcu' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Marcu\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	it("setting entity:3's forename to 'Mirco' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Mirco\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	it("setting entity:4's forename to 'Mirca' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Mirca\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	it("setting entity:5's forename to 'Mircu' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Mircu\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });

	it("setting entity:6's forename to 'Nircu' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Nircu\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	it("Using the Levenshtein distance filter", function(done) {
      client.write("SELECT $a $b WHERE { $a <forename> $b . FILTER lev($b, \"Marco\", 1)}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":[{a: '2', b:'Marcu'}, {a: '3', b:'Mirco'}]}}));
        done();
      });      
    });
	
	it("Using the Levenshtein distance filter", function(done) {
      client.write("SELECT $a $b WHERE { $a <forename> $b . FILTER lev($b, \"Marco\", 2)}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":[{a: '2', b:'Marcu'}, {a: '3', b:'Mirco'}, {a: '4', b:'Mirca'}, {a: '5', b:'Mircu'}]}}));
        done();
      });      
    });

	it("Using the Levenshtein distance filter", function(done) {
      client.write("SELECT $a $b WHERE { $a <forename> $b . FILTER lev($b, \"Marco\", 3)}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({"status":true,"errorCode":0,"info":"","result":{"type":"fsparql","data":[{a: '2', b:'Marcu'}, {a: '3', b:'Mirco'}, {a: '4', b:'Mirca'}, {a: '5', b:'Mircu'}, {a: '6', b:'Nircu'}]}}));
        done();
      });      
    });
	
	it("Sample query - logging out from admin", function(done) {
        client.write('USER LOGOUT');
        client.once('data', function(data) {
            var resultJSON = JSON.parse(data);

        done();
        });
    });
 });   
}); 