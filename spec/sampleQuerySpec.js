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
	it("setting entity:1's forename to 'Marco' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
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
       client.write("SELECT $a WHERE { $a <age> \"34\"}");
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
	
	it("flush everything", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });
	//test insert with 4 properties
	it("setting entity:1's forename to 'Marco' and surname to 'Reus', which is aged '28' and drinks 'Water'", function(done) {
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
	
	//test select - Option 5 entity <property> $c
    it("Checking if option 5 works - if the entity exists ", function(done) {
      client.write("SELECT $a WHERE { entity:1 <surname> $a}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'})]})}));
        done();
      });      
    });
	
	//test select - Option 5 entity <property> $c
    it("Checking if option 5 works - if the entity exists ", function(done) {
      client.write("SELECT $a $b WHERE { entity:1 <surname> $a . entity:2 <forename> $b}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'}), ({b: 'Moe'})]})}));
        done();
      });      
    });
	
	//test select - Option 5 entity <property> $c
    it("Checking if option 5 works - if the entity exists ", function(done) {
      client.write("SELECT $a $b $c WHERE { entity:1 <surname> $a . entity:2 <forename> $b . entity:3 <age> $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({a: 'Reus'}), ({b: 'Moe'}), ({c: '34'})]})}));
        done();
      });      
    });
	
	//test select - Option 8 entity $b $c 
    it("Checking if option 8 works - if the entity exists ", function(done) {
      client.write("SELECT $a WHERE { entity:4 $b $c}");
      client.once('data', function(data) {
		var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[]})}));
        done();
      });      
    });
	
	//test select - Option 8 entity $b $c 
    it("Checking if option 8 works - if the entity exists ", function(done) {
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

//need to flush everything at the start of the session to get rid of any previously stored data
	it("flush everything", function(done) {
      client.write("FLUSH");
      client.once('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Database cleared.'})}));
        done();
      });      
    });
	
    //test insert with 2 properties
    it("Adding 150 entities", function(done) {
      client.write("INSERT DATA {entity:1 <forename> \"Alexis1\"; <surname> \"Sanchez1\" . entity:2 <forename> \"Alexis2\"; <surname> \"Sanchez2\" . entity:3 <forename> \"Alexis3\"; <surname> \"Sanchez3\" . entity:4 <forename> \"Alexis4\"; <surname> \"Sanchez4\" . entity:5 <forename> \"Alexis5\"; <surname> \"Sanchez5\" . entity:6 <forename> \"Alexis6\"; <surname> \"Sanchez6\" . entity:7 <forename> \"Alexis7\"; <surname> \"Sanchez7\" . entity:8 <forename> \"Alexis8\"; <surname> \"Sanchez8\" . entity:9 <forename> \"Alexis9\"; <surname> \"Sanchez9\" . entity:10 <forename> \"Alexis10\"; <surname> \"Sanchez10\" . entity:11 <forename> \"Alexis11\"; <surname> \"Sanchez11\" . entity:12 <forename> \"Alexis12\"; <surname> \"Sanchez12\" . entity:13 <forename> \"Alexis13\"; <surname> \"Sanchez13\" . entity:14 <forename> \"Alexis14\"; <surname> \"Sanchez14\" . entity:15 <forename> \"Alexis15\"; <surname> \"Sanchez15\" . entity:16 <forename> \"Alexis16\"; <surname> \"Sanchez16\" . entity:17 <forename> \"Alexis17\"; <surname> \"Sanchez17\" . entity:18 <forename> \"Alexis18\"; <surname> \"Sanchez18\" . entity:19 <forename> \"Alexis19\"; <surname> \"Sanchez19\" . entity:20 <forename> \"Alexis20\"; <surname> \"Sanchez20\" . entity:21 <forename> \"Alexis21\"; <surname> \"Sanchez21\" . entity:22 <forename> \"Alexis22\"; <surname> \"Sanchez22\" . entity:23 <forename> \"Alexis23\"; <surname> \"Sanchez23\" . entity:24 <forename> \"Alexis24\"; <surname> \"Sanchez24\" . entity:25 <forename> \"Alexis25\"; <surname> \"Sanchez25\" . entity:26 <forename> \"Alexis26\"; <surname> \"Sanchez26\" . entity:27 <forename> \"Alexis27\"; <surname> \"Sanchez27\" . entity:28 <forename> \"Alexis28\"; <surname> \"Sanchez28\" . entity:29 <forename> \"Alexis29\"; <surname> \"Sanchez29\" . entity:30 <forename> \"Alexis30\"; <surname> \"Sanchez30\" . entity:31 <forename> \"Alexis31\"; <surname> \"Sanchez31\" . entity:32 <forename> \"Alexis32\"; <surname> \"Sanchez32\" . entity:33 <forename> \"Alexis33\"; <surname> \"Sanchez33\" . entity:34 <forename> \"Alexis34\"; <surname> \"Sanchez34\" . entity:35 <forename> \"Alexis35\"; <surname> \"Sanchez35\" . entity:36 <forename> \"Alexis36\"; <surname> \"Sanchez36\" . entity:37 <forename> \"Alexis37\"; <surname> \"Sanchez37\" . entity:38 <forename> \"Alexis38\"; <surname> \"Sanchez38\" . entity:39 <forename> \"Alexis39\"; <surname> \"Sanchez39\" . entity:40 <forename> \"Alexis40\"; <surname> \"Sanchez40\" . entity:41 <forename> \"Alexis41\"; <surname> \"Sanchez41\" . entity:42 <forename> \"Alexis42\"; <surname> \"Sanchez42\" . entity:43 <forename> \"Alexis43\"; <surname> \"Sanchez43\" . entity:44 <forename> \"Alexis44\"; <surname> \"Sanchez44\" . entity:45 <forename> \"Alexis45\"; <surname> \"Sanchez45\" . entity:46 <forename> \"Alexis46\"; <surname> \"Sanchez46\" . entity:47 <forename> \"Alexis47\"; <surname> \"Sanchez47\" . entity:48 <forename> \"Alexis48\"; <surname> \"Sanchez48\" . entity:49 <forename> \"Alexis49\"; <surname> \"Sanchez49\" . entity:50 <forename> \"Alexis50\"; <surname> \"Sanchez50\" . entity:51 <forename> \"Alexis51\"; <surname> \"Sanchez51\" . entity:52 <forename> \"Alexis52\"; <surname> \"Sanchez52\" . entity:53 <forename> \"Alexis53\"; <surname> \"Sanchez53\" . entity:54 <forename> \"Alexis54\"; <surname> \"Sanchez54\" . entity:55 <forename> \"Alexis55\"; <surname> \"Sanchez55\" . entity:56 <forename> \"Alexis56\"; <surname> \"Sanchez56\" . entity:57 <forename> \"Alexis57\"; <surname> \"Sanchez57\" . entity:58 <forename> \"Alexis58\"; <surname> \"Sanchez58\" . entity:59 <forename> \"Alexis59\"; <surname> \"Sanchez59\" . entity:60 <forename> \"Alexis60\"; <surname> \"Sanchez60\" . entity:61 <forename> \"Alexis61\"; <surname> \"Sanchez61\" . entity:62 <forename> \"Alexis62\"; <surname> \"Sanchez62\" . entity:63 <forename> \"Alexis63\"; <surname> \"Sanchez63\" . entity:64 <forename> \"Alexis64\"; <surname> \"Sanchez64\" . entity:65 <forename> \"Alexis65\"; <surname> \"Sanchez65\" . entity:66 <forename> \"Alexis66\"; <surname> \"Sanchez66\" . entity:67 <forename> \"Alexis67\"; <surname> \"Sanchez67\" . entity:68 <forename> \"Alexis68\"; <surname> \"Sanchez68\" . entity:69 <forename> \"Alexis69\"; <surname> \"Sanchez69\" . entity:70 <forename> \"Alexis70\"; <surname> \"Sanchez70\" . entity:71 <forename> \"Alexis71\"; <surname> \"Sanchez71\" . entity:72 <forename> \"Alexis72\"; <surname> \"Sanchez72\" . entity:73 <forename> \"Alexis73\"; <surname> \"Sanchez73\" . entity:74 <forename> \"Alexis74\"; <surname> \"Sanchez74\" . entity:75 <forename> \"Alexis75\"; <surname> \"Sanchez75\" . entity:76 <forename> \"Alexis76\"; <surname> \"Sanchez76\" . entity:77 <forename> \"Alexis77\"; <surname> \"Sanchez77\" . entity:78 <forename> \"Alexis78\"; <surname> \"Sanchez78\" . entity:79 <forename> \"Alexis79\"; <surname> \"Sanchez79\" . entity:80 <forename> \"Alexis80\"; <surname> \"Sanchez80\" . entity:81 <forename> \"Alexis81\"; <surname> \"Sanchez81\" . entity:82 <forename> \"Alexis82\"; <surname> \"Sanchez82\" . entity:83 <forename> \"Alexis83\"; <surname> \"Sanchez83\" . entity:84 <forename> \"Alexis84\"; <surname> \"Sanchez84\" . entity:85 <forename> \"Alexis85\"; <surname> \"Sanchez85\" . entity:86 <forename> \"Alexis86\"; <surname> \"Sanchez86\" . entity:87 <forename> \"Alexis87\"; <surname> \"Sanchez87\" . entity:88 <forename> \"Alexis88\"; <surname> \"Sanchez88\" . entity:89 <forename> \"Alexis89\"; <surname> \"Sanchez89\" . entity:90 <forename> \"Alexis90\"; <surname> \"Sanchez90\" . entity:91 <forename> \"Alexis91\"; <surname> \"Sanchez91\" . entity:92 <forename> \"Alexis92\"; <surname> \"Sanchez92\" . entity:93 <forename> \"Alexis93\"; <surname> \"Sanchez93\" . entity:94 <forename> \"Alexis94\"; <surname> \"Sanchez94\" . entity:95 <forename> \"Alexis95\"; <surname> \"Sanchez95\" . entity:96 <forename> \"Alexis96\"; <surname> \"Sanchez96\" . entity:97 <forename> \"Alexis97\"; <surname> \"Sanchez97\" . entity:98 <forename> \"Alexis98\"; <surname> \"Sanchez98\" . entity:99 <forename> \"Alexis99\"; <surname> \"Sanchez99\" . entity:100 <forename> \"Alexis100\"; <surname> \"Sanchez100\" . entity:101 <forename> \"Alexis101\"; <surname> \"Sanchez101\"; <profession> \"Bartender\" . entity:102 <forename> \"Alexis102\"; <surname> \"Sanchez102\"; <profession> \"Bartender\" . entity:103 <forename> \"Alexis103\"; <surname> \"Sanchez103\"; <profession> \"Bartender\" . entity:104 <forename> \"Alexis104\"; <surname> \"Sanchez104\"; <profession> \"Bartender\" . entity:105 <forename> \"Alexis105\"; <surname> \"Sanchez105\"; <profession> \"Bartender\" . entity:106 <forename> \"Alexis106\"; <surname> \"Sanchez106\"; <profession> \"Bartender\" . entity:107 <forename> \"Alexis107\"; <surname> \"Sanchez107\"; <profession> \"Bartender\" . entity:108 <forename> \"Alexis108\"; <surname> \"Sanchez108\"; <profession> \"Bartender\" . entity:109 <forename> \"Alexis109\"; <surname> \"Sanchez109\"; <profession> \"Bartender\" . entity:110 <forename> \"Alexis110\"; <surname> \"Sanchez110\"; <profession> \"Bartender\" . entity:111 <forename> \"Alexis111\"; <surname> \"Sanchez111\"; <profession> \"Bartender\" . entity:112 <forename> \"Alexis112\"; <surname> \"Sanchez112\"; <profession> \"Bartender\" . entity:113 <forename> \"Alexis113\"; <surname> \"Sanchez113\"; <profession> \"Bartender\" . entity:114 <forename> \"Alexis114\"; <surname> \"Sanchez114\"; <profession> \"Bartender\" . entity:115 <forename> \"Alexis115\"; <surname> \"Sanchez115\"; <profession> \"Bartender\" . entity:116 <forename> \"Alexis116\"; <surname> \"Sanchez116\"; <profession> \"Bartender\" . entity:117 <forename> \"Alexis117\"; <surname> \"Sanchez117\"; <profession> \"Bartender\" . entity:118 <forename> \"Alexis118\"; <surname> \"Sanchez118\"; <profession> \"Bartender\" . entity:119 <forename> \"Alexis119\"; <surname> \"Sanchez119\"; <profession> \"Bartender\" . entity:120 <forename> \"Alexis120\"; <surname> \"Sanchez120\"; <profession> \"Bartender\" . entity:121 <forename> \"Alexis121\"; <surname> \"Sanchez121\"; <profession> \"Bartender\" . entity:122 <forename> \"Alexis122\"; <surname> \"Sanchez122\"; <profession> \"Bartender\" . entity:123 <forename> \"Alexis123\"; <surname> \"Sanchez123\"; <profession> \"Bartender\" . entity:124 <forename> \"Alexis124\"; <surname> \"Sanchez124\"; <profession> \"Bartender\" . entity:125 <forename> \"Alexis125\"; <surname> \"Sanchez125\"; <profession> \"Bartender\" . entity:126 <forename> \"Alexis126\"; <surname> \"Sanchez126\"; <profession> \"Bartender\" . entity:127 <forename> \"Alexis127\"; <surname> \"Sanchez127\"; <profession> \"Bartender\" . entity:128 <forename> \"Alexis128\"; <surname> \"Sanchez128\"; <profession> \"Bartender\" . entity:129 <forename> \"Alexis129\"; <surname> \"Sanchez129\"; <profession> \"Bartender\" . entity:130 <forename> \"Alexis130\"; <surname> \"Sanchez130\"; <profession> \"Bartender\" . entity:131 <forename> \"Alexis131\"; <surname> \"Sanchez131\"; <profession> \"Bartender\" . entity:132 <forename> \"Alexis132\"; <surname> \"Sanchez132\"; <profession> \"Bartender\" . entity:133 <forename> \"Alexis133\"; <surname> \"Sanchez133\"; <profession> \"Bartender\" . entity:134 <forename> \"Alexis134\"; <surname> \"Sanchez134\"; <profession> \"Bartender\" . entity:135 <forename> \"Alexis135\"; <surname> \"Sanchez135\"; <profession> \"Bartender\" . entity:136 <forename> \"Alexis136\"; <surname> \"Sanchez136\"; <profession> \"Bartender\" . entity:137 <forename> \"Alexis137\"; <surname> \"Sanchez137\"; <profession> \"Bartender\" . entity:138 <forename> \"Alexis138\"; <surname> \"Sanchez138\"; <profession> \"Bartender\" . entity:139 <forename> \"Alexis139\"; <surname> \"Sanchez139\"; <profession> \"Bartender\" . entity:140 <forename> \"Alexis140\"; <surname> \"Sanchez140\"; <profession> \"Bartender\" . entity:141 <forename> \"Alexis141\"; <surname> \"Sanchez141\"; <profession> \"Bartender\" . entity:142 <forename> \"Alexis142\"; <surname> \"Sanchez142\"; <profession> \"Bartender\" . entity:143 <forename> \"Alexis143\"; <surname> \"Sanchez143\"; <profession> \"Bartender\" . entity:144 <forename> \"Alexis144\"; <surname> \"Sanchez144\"; <profession> \"Bartender\" . entity:145 <forename> \"Alexis145\"; <surname> \"Sanchez145\"; <profession> \"Bartender\" . entity:146 <forename> \"Alexis146\"; <surname> \"Sanchez146\"; <profession> \"Bartender\" . entity:147 <forename> \"Alexis147\"; <surname> \"Sanchez147\"; <profession> \"Bartender\" . entity:148 <forename> \"Alexis148\"; <surname> \"Sanchez148\"; <profession> \"Bartender\" . entity:149 <forename> \"Alexis149\"; <surname> \"Sanchez149\"; <profession> \"Bartender\" . entity:150 <forename> \"Alexis150\"; <surname> \"Sanchez150\"; <profession> \"Bartender\"}");
      client.on('data', function(data) {
        done();
      });      
    });
	
	//test retrieve
    it("Retrieving entities which have a profession set - 50 entities", function(done) {
      client.write("SELECT $a WHERE { $a <profession> $b}");
      client.on('data', function(data) {
        var resultJSON = JSON.parse(data);
         expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '101'}), ({ a: '102'}), ({ a: '103'}), ({ a: '104'}), ({ a: '105'}), ({ a: '106'}), ({ a: '107'}), ({ a: '108'}), ({ a: '109'}), ({ a: '110'}), ({ a: '111'}), ({ a: '112'}), ({ a: '113'}), ({ a: '114'}), ({ a: '115'}), ({ a: '116'}), ({ a: '117'}), ({ a: '118'}), ({ a: '119'}), ({ a: '120'}), ({ a: '121'}), ({ a: '122'}), ({ a: '123'}), ({ a: '124'}), ({ a: '125'}), ({ a: '126'}), ({ a: '127'}), ({ a: '128'}), ({ a: '129'}), ({ a: '130'}), ({ a: '131'}), ({ a: '132'}), ({ a: '133'}), ({ a: '134'}), ({ a: '135'}), ({ a: '136'}), ({ a: '137'}), ({ a: '138'}), ({ a: '139'}), ({ a: '140'}), ({ a: '141'}), ({ a: '142'}), ({ a: '143'}), ({ a: '144'}), ({ a: '145'}), ({ a: '146'}), ({ a: '147'}), ({ a: '148'}), ({ a: '149'}), ({ a: '150'})]})}));
        done();
      });      
    });
	
		/*
	//test retrieve
    it("Retrieving entities which have the forename set - 150 entities", function(done) {
      client.write("SELECT $a WHERE { $a <forename> $b}");
      client.on('data', function(data) {
        var resultJSON = JSON.parse(data);
        expect(resultJSON).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '1'}), ({ a: '2'}), ({ a: '3'}), ({ a: '4'}), ({ a: '5'}), ({ a: '6'}), ({ a: '7'}), ({ a: '8'}), ({ a: '9'}), ({ a: '10'}), ({ a: '11'}), ({ a: '12'}), ({ a: '13'}), ({ a: '14'}), ({ a: '15'}), ({ a: '16'}), ({ a: '17'}), ({ a: '18'}), ({ a: '19'}), ({ a: '20'}), ({ a: '21'}), ({ a: '22'}), ({ a: '23'}), ({ a: '24'}), ({ a: '25'}), ({ a: '26'}), ({ a: '27'}), ({ a: '28'}), ({ a: '29'}), ({ a: '30'}), ({ a: '31'}), ({ a: '32'}), ({ a: '33'}), ({ a: '34'}), ({ a: '35'}), ({ a: '36'}), ({ a: '37'}), ({ a: '38'}), ({ a: '39'}), ({ a: '40'}), ({ a: '41'}), ({ a: '42'}), ({ a: '43'}), ({ a: '44'}), ({ a: '45'}), ({ a: '46'}), ({ a: '47'}), ({ a: '48'}), ({ a: '49'}), ({ a: '50'}), ({ a: '51'}), ({ a: '52'}), ({ a: '53'}), ({ a: '54'}), ({ a: '55'}), ({ a: '56'}), ({ a: '57'}), ({ a: '58'}), ({ a: '59'}), ({ a: '60'}), ({ a: '61'}), ({ a: '62'}), ({ a: '63'}), ({ a: '64'}), ({ a: '65'}), ({ a: '66'}), ({ a: '67'}), ({ a: '68'}), ({ a: '69'}), ({ a: '70'}), ({ a: '71'}), ({ a: '72'}), ({ a: '73'}), ({ a: '74'}), ({ a: '75'}), ({ a: '76'}), ({ a: '77'}), ({ a: '78'}), ({ a: '79'}), ({ a: '80'}), ({ a: '81'}), ({ a: '82'}), ({ a: '83'}), ({ a: '84'}), ({ a: '85'}), ({ a: '86'}), ({ a: '87'}), ({ a: '88'}), ({ a: '89'}), ({ a: '90'}), ({ a: '91'}), ({ a: '92'}), ({ a: '93'}), ({ a: '94'}), ({ a: '95'}), ({ a: '96'}), ({ a: '97'}), ({ a: '98'}), ({ a: '99'}), ({ a: '100'}), ({ a: '101'}), ({ a: '102'}), ({ a: '103'}), ({ a: '104'}), ({ a: '105'}), ({ a: '106'}), ({ a: '107'}), ({ a: '108'}), ({ a: '109'}), ({ a: '110'}), ({ a: '111'}), ({ a: '112'}), ({ a: '113'}), ({ a: '114'}), ({ a: '115'}), ({ a: '116'}), ({ a: '117'}), ({ a: '118'}), ({ a: '119'}), ({ a: '120'}), ({ a: '121'}), ({ a: '122'}), ({ a: '123'}), ({ a: '124'}), ({ a: '125'}), ({ a: '126'}), ({ a: '127'}), ({ a: '128'}), ({ a: '129'}), ({ a: '130'}), ({ a: '131'}), ({ a: '132'}), ({ a: '133'}), ({ a: '134'}), ({ a: '135'}), ({ a: '136'}), ({ a: '137'}), ({ a: '138'}), ({ a: '139'}), ({ a: '140'}), ({ a: '141'}), ({ a: '142'}), ({ a: '143'}), ({ a: '144'}), ({ a: '145'}), ({ a: '146'}), ({ a: '147'}), ({ a: '148'}), ({ a: '149'}), ({ a: '150'})]})}));
        done();
      });      
    });
	*/
	
	});
});
