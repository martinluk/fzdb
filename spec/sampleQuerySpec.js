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
	
	
    //test insert
    it("setting entity:2's forname to Fred", function(done) {
      client.write("INSERT DATA { $a <forename> \"Fred\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });

    //test retrieve
    it("getting the forename of entity:2", function(done) {
      client.write("SELECT $a WHERE { entity:2 <forename> $a }");
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
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'})]})}));
        done();
      });      
    });

	//test insert with 2 properties
    it("setting entity:3's forename to 'Ned' and surname to 'Flanders'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Ned\"; <surname> \"Flanders\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test retrieve
    it("getting the surname of entity:3", function(done) {
      client.write("SELECT $a WHERE { entity:3 <surname> $a }");
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
	it("setting entity:2's forename to 'Marco' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Marco\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test insert with 5 properties
    it("setting entity:3's forename to 'Moe' and surname to 'Szyslak', which is aged '34', drinks 'Beer' and has as a profession 'Bartender'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Moe\"; <surname> \"Szyslak\"; <age> \"34\"; <drinks> \"Beer\"; <profession> \"Bartender\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });

	//test insert with 4 properties
	it("setting entity:4's forename to 'Marco' and surname to 'Polo', which is aged '34', drinks 'Wine'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Marco\"; <surname> \"Polo\"; <age> \"34\"; <drinks> \"Wine\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test select - Option 1  $a <prop> value
    it("getting entities with forename Marco", function(done) {
      client.write("SELECT $a WHERE { $a <forename> \"Marco\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'}), ({a: '4'})]})}));
        done();
      });      
    });
	
	//test select - Option 1  $a <prop> value
    it("getting entities with forename Marco and surname Reus", function(done) {
      client.write("SELECT $a WHERE { $a <forename> \"Marco\" . $a <surname> \"Reus\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'})]})}));
        done();
      });      
    });
	
	//test select - Option 1  $a <prop> value
    it("getting entities with age 34", function(done) {
       client.write("SELECT $a WHERE { $a <age> \"34\"}");
       client.once('data', function(data) {
	     var resultJSON = JSON.parse(data);
         expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '3'}), ({a: '4'})]})}));
         done();
       });      
    });
	
	//test select - Option 2 $a <prop> $b
	it("getting entities with surname Polo and age 34 - no entities", function(done) {
       client.write("SELECT $a WHERE { $a <age> \"34\" . $a <surname> \"Polo\"}");
       client.once('data', function(data) {
	     var resultJSON = JSON.parse(data);
         expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '4'})]})}));
         done();
       });      
    });
	
	//test select - Option 2 $a <prop> $b
	it("getting entities with surname Reus and age 34", function(done) {
       client.write("SELECT $a WHERE { $a <age> \"34\" . $a <surname> \"Reus\"}");
       client.once('data', function(data) {
	     var resultJSON = JSON.parse(data);
         expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[]})}));
         done();
       });      
    });
	
	//test select - Option 2 $a <prop> $b
    it("In this case, select option 2, should retrieve all three entities, as all three have the drinks property set", function(done) {
      client.write("SELECT $a WHERE { $a <drinks> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'}), ({a: '3'}), ({a: '4'})]})}));
        done();
      });      
    });
	
	//test select - Option 2 $a <prop> $b - FAULTY, SHOULD RETURN NOTHING
    it("In this case, select option 2, should retrieve entity 3, as it is the only one which has both the drinks and profession properties set", function(done) {
      client.write("SELECT $a WHERE { $a <drinks> $b . $a <profession> $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '3'})]})}));
        done();
      });      
    });
	
	//test select - Option 2 $a <prop> $b
    it("In this case, select option 2, should retrieve nothing, but it returns entity 3", function(done) {
      client.write("SELECT $a WHERE { $a <drinks> $b . $a <profession> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[]})}));
        done();
      });   
    });


	//test select - Option 2 $a <prop> $b
    it("In this case, select option 2, should retrieve the drinks value and the profession value", function(done) {
      client.write("SELECT $b $c WHERE { $a <drinks> $b . $a <profession> $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({b: 'Beer', c: 'Bartender'})]})}));
        done();
      });      
    });
	
	//test select - Option 2 $a <prop> $b
	it("In this case, select option 2, should retrieve entity 3, as it is the only one which has both the drinks and profession properties set", function(done) {
      client.write("SELECT $a WHERE { $a <drinks> $b . $a <profession> $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '3'})]})}));
        done();
      });      
    });
	
	//test select - Option 2 $a <prop> $b
    it("Testing select option 2, should retrieve the profession of entity 3, which is a Bartender", function(done) {
      client.write("SELECT $b WHERE { $a <profession> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({b: 'Bartender'})]})}));
        done();
      });      
    });
	
	//test select - Option 2 $a <prop> $b
    it("Testing select option 2, should retrieve only entity 3, which has a profession property set", function(done) {
      client.write("SELECT $a WHERE { $a <profession> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '3'})]})}));
        done();
      });      
    });
	
	it("Testing select option 2, should retrieve only entities 2, 3 and 4, which have a drinks property set", function(done) {
      client.write("SELECT $a WHERE { $a <drinks> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '2'}), ({a: '3'}), ({a: '4'})]})}));
        done();
      });      
    });
	
	//test select - Option 3 $a $b value 
    it("Select entities which contain Marco ", function(done) {
      client.write("SELECT $a WHERE { $a $b \"Marco\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: '2'}), ({a: '4'})]})}));
        done();
      });      
    });

	//test select - Option 3 $a $b value 
    it("Select entities which have a property which is equal to both Marco and 34", function(done) {
      client.write("SELECT $a WHERE { $a $b \"Marco\" . $a $b \"34\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[]}});
        done();
      });      
    });
	
	//test select - Option 3 $a $b value 
    it("Select properties which are equal to both Marco and 34 for the same entity", function(done) {
      client.write("SELECT $b WHERE { $a $b \"Marco\" . $a $b \"34\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[]}});
        done();
      });      
    });
    
	//test select - Option 6 entity <prop> $c
    it("Retrieving the surname of entity 2", function(done) {
      client.write("SELECT $a WHERE { entity:2 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
	it("Retrieving the surnames of entities 2 and 3", function(done) {
      client.write("SELECT $a $b WHERE { entity:2 <surname> $a . entity:3 <surname> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'}), ({b: 'Szyslak'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
	it("Retrieve the common surname of entity:2 and entity:3", function(done) {
      client.write("SELECT $a WHERE { entity:2 <surname> $a . entity:3 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: {type: 'fsparql', data:[]}}));
        done();
      });      
    }); 
	
	//test select - Option 6 entity <prop> $c
    it("Retrieving drinks property of entity 2", function(done) {
      client.write("SELECT $a WHERE { entity:2 <drinks> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Water'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
    it("Retrieving the surname of entity 3", function(done) {
      client.write("SELECT $a WHERE { entity:3 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Szyslak'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
    it("Retrieving the profession property of entity 3", function(done) {
      client.write("SELECT $a WHERE { entity:3 <profession> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Bartender'})]})}));
        done();
      });      
    });
	
	//test select - Option 6 entity <prop> $c
    it("Checking if empty result is returned if the property does not exist for the entity", function(done) {
      client.write("SELECT $a WHERE { entity:2 <profession> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[]})}));
        done();
      });      
    });

	//test select - Option 7 entity $b value 
    it("Checking if option 7 returns the property that has Marco as a value ", function(done) {
      client.write("SELECT $a WHERE { entity:2 $a \"Marco\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'forename'})]})}));
        done();
      });      
    });
	
	
	it("Checking if option 7 returns for entity 2, the property that has Marco as a value and that it returns for entity 3 the property that has 34 as a value", function(done) {
      client.write("SELECT $a $b WHERE { entity:2 $a \"Marco\" . entity:3 $b \"34\"}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'forename'}), ({b: 'age'})]})}));
        done();
      });      
    });
	
	
	it("flush everything", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });
	//test insert with 4 properties
	it("setting entity:2's forename to 'Marco' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Marco\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test insert with 5 properties
    it("setting entity:3's forename to 'Moe' and surname to 'Szyslak', which is aged '34', drinks 'Beer' and has as a profession 'Bartender'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Moe\"; <surname> \"Szyslak\"; <age> \"34\"; <drinks> \"Beer\"; <profession> \"Bartender\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });

	//test insert with 4 properties
	it("setting entity:4's forename to 'Marco' and surname to 'Polo', which is aged '34', drinks 'Wine'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Marco\"; <surname> \"Polo\"; <age> \"34\"; <drinks> \"Wine\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test select - Option 5 entity <property> $c
    it("Checking if option 5 works - if the entity exists ", function(done) {
      client.write("SELECT $a WHERE { entity:2 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'})]})}));
        done();
      });      
    });
	
	//test select - Option 5 entity <property> $c
    it("Checking if option 5 works - if the entity exists ", function(done) {
      client.write("SELECT $a $b WHERE { entity:2 <surname> $a . entity:3 <forename> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'}), ({b: 'Moe'})]})}));
        done();
      });      
    });
	
	//test select - Option 5 entity <property> $c
    it("Checking if option 5 works - if the entity exists ", function(done) {
      client.write("SELECT $a $b $c WHERE { entity:2 <surname> $a . entity:3 <forename> $b . entity:4 <age> $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'}), ({b: 'Moe'}), ({c: '34'})]})}));
        done();
      });      
    });
	
	//test select - Option 8 entity $b $c 
    it("Checking if option 8 works - if the entity exists ", function(done) {
      client.write("SELECT $a WHERE { entity:5 $b $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[]})}));
        done();
      });      
    });
	
	//test select - Option 8 entity $b $c 
    it("Checking if option 8 works - if the entity exists ", function(done) {
      client.write("SELECT $a WHERE { entity:2 $b $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[]})}));
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
	
	var assignedIds = {};

  //Multiple insert working
  it("having multiple inserts'", function(done) {
      client.write("INSERT DATA { $per1 <forename> \"Homer\", [60] \"Max\"; <surname> \"Simpson\", [60] \"Power\"; <age> 38 ; <wife> $per2; <drinks> \"Beer\" . $per2 <forename> \"Marge\"; <surname> \"Simpson\", [40] \"Bouvier\"; <age> 34 . $per3 <forename> \"Ned\"; <surname> \"Flanders\" . $per4 <forename> \"Moe\"; <surname> \"Szyslak\"; <occupation> \"Bartender\" }  WHERE { NEW($per1,\"person\") .  NEW($per2,\"person\") . NEW($per3,\"person\") . NEW($per4,\"person\") }");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        assignedIds = resultJSON.result.data;
        done();
      });      
    });
  
  //Multiple insert working
  it("having multiple inserts'", function(done) {
      client.write("INSERT DATA { $per1 <forename> \"Phil\", [60] \"Max\"; <surname> \"Travis\", [60] \"Power\"; <age> 38 ; <wife> $per2; <drinks> \"Beer\" . $per2 <forename> \"Marge\"; <surname> \"Sinclair\", [40] \"Dentist\"; <age> 37 . $per3 <forename> \"Barney\"; <surname> \"Stinson\"; <profession> \"magician\" . $per4 <forename> \"Moe\"; <surname> \"Szyslak\"; <occupation> \"Bartender\" } WHERE { NEW($per1,\"person\") . NEW($per2,\"person\") . NEW($per3,\"person\") . NEW($per4,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
  
  //testing if entity is returned as output
    it("Retrieving the wife of entity:2 which is entity:3", function(done) {
      client.write("SELECT $a WHERE { entity:" + assignedIds["per1"] + " <wife> $a}");
      client.once('data', function(data) {
    var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: assignedIds["per2"]})]})}));
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
	it("setting entity:2's forename to 'Marco' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
      client.write("INSERT DATA { $a <forename> \"Marco\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" } WHERE { NEW($a,\"person\") }");
      client.once('data', function(data) {
        done();
      });      
    });
	
	//test select - Option 5 entity <property> $c
    it("Checking if option 5 works - if the entity exists ", function(done) {
      client.write("SELECT $a WHERE { entity:2 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'})]})}));
        done();
      });      
    });
	
	
//need to flush everything at the start of the session to get rid of any previously stored data
	it("NANA", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });   
	
	});
});
