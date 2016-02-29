var net = require('net');

describe("Fuzzy Database", function() {
  var client;

  //connects to the database
  beforeAll(function(done) { 
    client = new net.Socket();
    client.connect(1407, '127.0.0.1', function() {
      client.write("FLUSH");
      client.once('data', function(data) {
        done();
      });   
    });
  });
  
  describe("sends the command over TCP", function() {

    //tests are run sequentially

    //test pong
    it("'PONG'", function(done) {
      client.write("PING");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON.result).toEqual({type: 'text', data: 'PONG'});
        done();
      });      
    });

	//need to flush everything at the start of the session to get rid of any previously stored data
	it("flush everything", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });
	
    //test insert
    it("setting entity:1's forname to Fred", function(done) {
      client.write("INSERT DATA { entity:1 <forename> \"Fred\" }");
      client.once('data', function(data) {
        done();
      });      
    });

    //test retrieve
    it("getting the forename of entity:1", function(done) {
      client.write("SELECT $a WHERE { entity:1 <forename> $a }");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: 'Fred'})]})}));
        done();
      });      
    });

    it("getting entities with forename 'Fred'", function(done) {
      client.write("SELECT $a WHERE { $a <forename> \"Fred\" }");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '1'})]})}));
        done();
      });      
    });

	//test insert with 2 properties
    it("setting entity:2's forename to 'Ned' and surname to 'Flanders'", function(done) {
      client.write("INSERT DATA { entity:2 <forename> \"Ned\"; <surname> \"Flanders\" }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test retrieve
    it("getting the surname of entity:2", function(done) {
      client.write("SELECT $a WHERE { entity:2 <surname> $a }");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: 'Flanders'})]})}));
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
	
	//test insert with 4 properties
	it("setting entity:1's forename to 'Marco' and surname to 'Reus', which is aged '28', drinks 'Water'", function(done) {
      client.write("INSERT DATA { entity:1 <forename> \"Marco\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test insert with 5 properties
    it("setting entity:2's forename to 'Moe' and surname to 'Szyslak', which is aged '34', drinks 'Beer' and has as a profession 'Bartender'", function(done) {
      client.write("INSERT DATA { entity:2 <forename> \"Moe\"; <surname> \"Szyslak\"; <age> \"34\"; <drinks> \"Beer\"; <profession> \"Bartender\" }");
      client.once('data', function(data) {
        done();
      });      
    });

	//test insert with 4 properties
	it("setting entity:3's forename to 'Marco' and surname to 'Polo', which is aged '34', drinks 'Wine'", function(done) {
      client.write("INSERT DATA { entity:3 <forename> \"Marco\"; <surname> \"Polo\"; <age> \"34\"; <drinks> \"Wine\" }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test select - Option 1  $a <prop> value
    it("getting entities with forename Marco", function(done) {
      client.write("SELECT $a WHERE { $a <forename> \"Marco\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '1'}), ({a: '3'})]})}));
        done();
      });      
    });
	
	//test select - Option 1  $a <prop> value
    it("getting entities with age 34", function(done) {
      client.write("SELECT $a WHERE { $a <age> 34}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'}), ({a: '3'})]})}));
        done();
      });      
    });
	
	//test select - Option 2 $a <prop> $b
    it("In this case, select option 2, should retrieve all three entities, as all three have the drinks property set", function(done) {
      client.write("SELECT $a WHERE { $a <drinks> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '1'}), ({a: '2'}), ({a: '3'})]})}));
        done();
      });      
    });

	//test select - Option 2 $a <prop> $b
    it("Testing select option 2, should retrieve only entity 2, which has a profession property set", function(done) {
      client.write("SELECT $a WHERE { $a <profession> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '2'})]})}));
        done();
      });      
    });
	
	//test select - Option 3 $a $b value 
    it("Select entities which contain Marco ", function(done) {
      client.write("SELECT $a WHERE { $a $b \"Marco\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '1'}), ({a: '3'})]})}));
        done();
      });      
    });

	//test select - Option 6 entity <prop> $c
    it("Retrieving the surname of entity 1", function(done) {
      client.write("SELECT $a WHERE { entity:1 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
    it("Retrieving drinks property of entity 1", function(done) {
      client.write("SELECT $a WHERE { entity:1 <drinks> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Water'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
    it("Retrieving the surname of entity 2", function(done) {
      client.write("SELECT $a WHERE { entity:2 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Szyslak'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
    it("Retrieving the profession property of entity 2", function(done) {
      client.write("SELECT $a WHERE { entity:2 <profession> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Bartender'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
    it("Checking if empty result is returned if the property does not exist for the entity", function(done) {
      client.write("SELECT $a WHERE { entity:1 <profession> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[]})}));
        done();
      });      
    });

	//test select - Option 7 entity $b value 
    it("Checking if option 7 returns if entity 1 contains Marco ", function(done) {
      client.write("SELECT $a WHERE { entity:1 $a \"Marco\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '1'})]})}));
        done();
      });      
    });
	
	//test select - Option 8 entity $b $c 
    it("Checking if option 8 returns the not implemented response ", function(done) {
      client.write("SELECT $a WHERE { entity:1 $b $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '1'})]})}));
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
	
	//Multiple insert working
	it("having multiple inserts'", function(done) {
      client.write("INSERT DATA { entity:1 <forename> \"Homer\", [60] \"Max\"; <surname> \"Simpson\", [60] \"Power\"; <age> 38 ; <wife> \"Marge\"; <drinks> \"Beer\" . entity:2 <forename> \"Marge\"; <surname> \"Simpson\", [40] \"Bouvier\"; <age> 34 . entity:3 <forename> \"Ned\"; <surname> \"Flanders\" . entity:4 <forename> \"Moe\"; <surname> \"Szyslak\"; <occupation> \"Bartender\" } ");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//Multiple insert working
	it("having multiple inserts'", function(done) {
      client.write("INSERT DATA { entity:5 <forename> \"Phil\", [60] \"Max\"; <surname> \"Travis\", [60] \"Power\"; <age> 38 ; <wife> \"entity:2\"; <drinks> \"Beer\" . entity:6 <forename> \"Marge\"; <surname> \"Sinclair\", [40] \"Dentist\"; <age> 37 . entity:7 <forename> \"Barney\"; <surname> \"Stinson\"; <profession> \"magician\" . entity:8 <forename> \"Moe\"; <surname> \"Szyslak\"; <occupation> \"Bartender\" } ");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c -- here database crashes if I try to retrieve the wife of entity:1 which is
	//entity:2. Probably because of having entity:2 as a wife and not an actual name
    it("Retrieving the profession property of entity 2", function(done) {
      client.write("SELECT $a WHERE { entity:5 <wife> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'entity:2'})]})}));
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

	});
});
