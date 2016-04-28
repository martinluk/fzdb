var h = require('./support/helper.js');

var liverpoolHistoricalData = h.getData("liverpool_full.fuz");

describe("Fuzzy Database: Liverpool Historical Data", function() { 

  
  describe("with liverpool fuz data loaded : ", function() {


  beforeAll(function(done) {
    h.setupClient();
    h.sendCmd(h.loginToAdminQuery).then(function() {
      h.sendCmd('FLUSH').then(function() {
        h.sendCmd(liverpoolHistoricalData).then(function() {
          h.sendCmd('USER LOGOUT').then(function() {done();});
        });
      });
    });
  }, 400000);

    //simple check of basic meta functionality
    describe("Tests for manipulating entities : ", function() {

      h.testCase("Select entities which have the forename set to Mariano", 'SELECT $a WHERE { $a <forename> "Mariano" }',
        h.resultTemplate([
           {"a":"2209"}, {"a":"2209"}, {"a":"2505"}, {"a":"2505"}, {"a":"3145"}, {"a":"3145"}, {"a":"3267"}, {"a":"3267"}, {"a":"3755"}, {"a":"3755"}
        ]));

      h.testCase("Select entities which have the birthplace property set and which have the forename set to Mariano", 'SELECT $a WHERE { $a <birthplace> $d . $a <forename> "Mariano" }',
        h.resultTemplate([
          {"a":"2209"}, {"a":"2209"}, {"a":"2505"}, {"a":"2505"}, {"a":"3145"}, {"a":"3145"}, {"a":"3267"}, {"a":"3267"}, {"a":"3755"}, {"a":"3755"}, {"a":"3267"}, {"a":"3267"}, {"a":"2209"}, {"a":"2209"}
        ]), 200000);
		
	  h.testCase("Select entities which have the one of the property values Cambello and the forename property set to Miguel", 'SELECT $a WHERE { $a $b "Cambello" . $a <forename> "Miguel" }',
      h.resultTemplate([
        {"a":"3269"}, {"a":"3269"}, {"a":"3269"}, {"a":"3269"}, {"a":"3269"}, {"a":"3269"}
      ]), 200000);
	  
	   h.testCase("Retrieving the surname of entity 2209", 'SELECT $a WHERE { entity:2209 <surname> $a}',
       h.resultTemplate([
        {"a":"Abad"}, {"a":"Abad"}
       ]), 200000);
	
	   h.testCase("Retrieving the property which has as a value Abad for entity 2209", 'SELECT $a WHERE { entity:2209 $a "Abad"}',
       h.resultTemplate([
        {"a":"surname"}, {"a":"surname"}
       ]), 200000);
	   
	   h.testCase("Retrieving the property names and values of entity 3", 'SELECT $a $b WHERE { entity:3 $a $b}',
       h.resultTemplate([
        {"a":"name", b:"Seaman"}, {"a":"type", b:"occupation"}, {"a":"subsetOf", b:"2"}
       ]), 200000);
	  
	   
	   h.testCase("Retrieving the property names and values of entity 2209", 'SELECT $a $b WHERE { entity:2209 $a $b}',
       h.resultTemplate([
        {"a":"type", b:"person"}, {"a":"forename", b:"Mariano"}, {"a":"forename", b:"Mariano"}, {"a":"birthdate", b:"31/12/1830"}, {"a":"birthdate", b:"01/01/1830"}, {"a":"sex", b:"m"}, {"a":"sex", b:"m"}, {"a":"surname", b:"Abad"}, {"a":"surname", b:"Abad"}, {"a":"birthplace", b:"1804"}, {"a":"birthplace", b:"1804"}, {"a":"address", b:"1296"}, {"a":"occupation", b:"1113"}, {"a":"Co-boarder", b:"2233"}, {"a":"Co-boarder", b:"3239"}, {"a":"Co-boarder", b:"3383"}, {"a":"Co-boarder", b:"3384"}, {"a":"Co-boarder", b:"3718"}, {"a":"Co-boarder", b:"3719"}, {"a":"Co-boarder", b:"2660"}, {"a":"Co-boarder", b:"2829"}, {"a":"Boarder", b:"2709"}
       ]), 200000);
	   
	   h.testCase("Returning the Boarder for entity 2208 which is person1449", 'SELECT $a WHERE {entity:2208 <Boarder> $a}',
       h.resultTemplate([
        
       ]), 200000);
	   
	  
	   h.testCase("Linking entities 2209 and 3310", 'LINK entity:2209 entity:3310',
       ({status: true, errorCode: 0, info:'', result: ({type: 'text', data:'Entities 2209 and 3310 linked successfully.'})}), 200000);

	   h.testCase("Retrieving the entities which have the surname set to Abaitua Allende-Salazar after linking", 'SELECT $a WHERE { $a <surname> "Abaitua Allende-Salazar"}',
       h.resultTemplate([
        {"a":"2209"}, {"a":"3199"}
       ]), 200000);
	   
	   h.testCase("Retrieving the entities which have the surname property set to Abad after linking", 'SELECT $a WHERE { $a <surname> "Abad"}',
       h.resultTemplate([
        {"a":"2209"}, {"a":"2209"}
       ]), 200000);
	   
	      h.testCase("Unlinking entities 2209 and 3310", 'UNLINK entity:2209 entity:3310',
       ({status: true, errorCode: 0, info:'', result: ({type: 'text', data:'Entities 2209 and 3310 unlinked successfully.'})}), 200000);

	   h.testCase("Retrieving the entities which have the surname set to Abaitua Allende-Salazar after unlinking", 'SELECT $a WHERE { $a <surname> "Abaitua Allende-Salazar"}',
       h.resultTemplate([
        {"a":"3199"}, {"a":"3310"}
       ]), 200000);
	   
	   h.testCase("Retrieving the entities which have the surname property set to Abad after unlinking", 'SELECT $a WHERE { $a <surname> "Abad"}',
       h.resultTemplate([
        {"a":"2209"}, {"a":"2209"}
       ]), 200000);
	   
	   
	   	h.testCase("Merging entities 2209 and 3310", 'LINK FINAL entity:2209 entity:3310',
       ({status: true, errorCode: 0, info:'', result: ({type: 'text', data:'Entities 2209 and 3310 merged successfully.'})}), 200000);

	   h.testCase("Retrieving the entity which has the surname set to Abaitua Allende-Salazar after merging", 'SELECT $a WHERE { $a <surname> "Abaitua Allende-Salazar"}',
       h.resultTemplate([
        {"a":"2209"}, {"a":"3199"}
       ]), 200000);
	   
	   h.testCase("Retrieving the entity which has the surname set to Abad after merging", 'SELECT $a WHERE { $a <surname> "Abad"}',
       h.resultTemplate([
        {"a":"2209"}, {"a":"2209"}
       ]), 200000);
	   
	   h.testCase("Retrieving the property-value pairs of entity 3309 after merging entities 2209 and 3310", 'SELECT $a $b WHERE { entity:3310 $a $b}',
       h.resultTemplate([

       ]), 200000);
	   
	   h.testCase("Testing meta property", 'SELECT $a $c WHERE { META $b { entity:2209 <surname> $c} . $b <fuz:source> $a }',
       h.resultTemplate([
       {"a":"3977", "c":"Abad"}, {"a":"3977", "c":"Abad"}, {"a":"1", "c":"Abaitua"}, {"a":"3977", "c":"Abaitua Allende-Salazar"}, {"a":"3979", "c":"Abaitua"}
       ]), 200000);
	   
	});		
  });
}); 