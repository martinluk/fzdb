var h = require('./support/helper.js');

var liverpoolHistoricalData = h.getData("liverpool.fuz");

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
  }, 200000);

    //simple check of basic meta functionality
    describe("Tests for manipulating entities : ", function() {

      h.testCase("Select entities which have the forename set to Mariano", 'SELECT $a WHERE { $a <forename> "Mariano" }',
        h.resultTemplate([
           {"a":"2208"}, {"a":"2208"}, {"a":"2504"}, {"a":"2504"}, {"a":"3144"}, {"a":"3144"}, {"a":"3266"}, {"a":"3266"}, {"a":"3754"}, {"a":"3754"}
        ]));

      h.testCase("Select entities which have the birthplace property set and which have the forename set to Mariano", 'SELECT $a WHERE { $a <birthplace> $d . $a <forename> "Mariano" }',
        h.resultTemplate([
          {"a":"2208"}, {"a":"2208"}
        ]), 200000);
		
	  h.testCase("Select entities which have the birthplace property set and which have the forename set to Mariano", 'SELECT $a WHERE { $a $b "Cambello" . $a <forename> "Miguel" }',
      h.resultTemplate([
        {"a":"3268"}, {"a":"3268"}, {"a":"3268"}, {"a":"3268"}, {"a":"3268"}, {"a":"3268"}
      ]), 200000);
	  
	   h.testCase("Retrieving the surname of entity 2208", 'SELECT $a WHERE { entity:2208 <surname> $a}',
       h.resultTemplate([
        {"a":"Abad"}, {"a":"Abad"}
       ]), 200000);
	
	   h.testCase("Retrieving the property which has as a value Abad for entity 2208", 'SELECT $a WHERE { entity:2208 $a "Abad"}',
       h.resultTemplate([
        {"a":"surname"}, {"a":"surname"}
       ]), 200000);
	   
	   h.testCase("Retrieving the property names and values of entity 2", 'SELECT $a $b WHERE { entity:2 $a $b}',
       h.resultTemplate([
        {"a":"name", b:"Maritime"}, {"a":"name", b:"Mariner"}, {"a":"type", b:"occupation"}
       ]), 200000);
	   
	   h.testCase("Retrieving the property names and values of entity 2208", 'SELECT $a $b WHERE { entity:2208 $a $b}',
       h.resultTemplate([
        {"a":"type", b:"person"}, {"a":"forename", b:"Mariano"}, {"a":"forename", b:"Mariano"}, {"a":"birthdate", b:"31/12/0031"}, {"a":"birthdate", b:"01/01/0001"}, {"a":"sex", b:"m"}, {"a":"sex", b:"m"}, {"a":"surname", b:"Abad"}, {"a":"surname", b:"Abad"}, {"a":"birthplace", b:"1803"}
       ]), 200000);
	   
	   h.testCase("Returning the Boarder for entity 2208 which is person1449", 'SELECT $a WHERE {entity:2208 <Boarder> $a}',
       h.resultTemplate([
        
       ]), 200000);
	   
	   
	   h.testCase("Linking entities 2208 and 3309", 'LINK entity:2208 entity:3309',
       ({status: true, errorCode: 0, info:'', result: ({type: 'text', data:'Entities 2208 and 3309 linked successfully.'})}), 200000);

	   h.testCase("Retrieving the entities which have the surname set to Abaitua Allende-Salazar after linking", 'SELECT $a WHERE { $a <surname> "Abaitua Allende-Salazar"}',
       h.resultTemplate([
        {"a":"2208"}, {"a":"3198"}
       ]), 200000);
	   
	   h.testCase("Retrieving the entities which have the surname property set to Abad after linking", 'SELECT $a WHERE { $a <surname> "Abad"}',
       h.resultTemplate([
        {"a":"2208"}, {"a":"2208"}
       ]), 200000);
	   
	      h.testCase("Unlinking entities 2208 and 3309", 'UNLINK entity:2208 entity:3309',
       ({status: true, errorCode: 0, info:'', result: ({type: 'text', data:'Entities 2208 and 3309 unlinked successfully.'})}), 200000);

	   h.testCase("Retrieving the entities which have the surname set to Abaitua Allende-Salazar after unlinking", 'SELECT $a WHERE { $a <surname> "Abaitua Allende-Salazar"}',
       h.resultTemplate([
        {"a":"3198"}, {"a":"3309"}
       ]), 200000);
	   
	   h.testCase("Retrieving the entities which have the surname property set to Abad after unlinking", 'SELECT $a WHERE { $a <surname> "Abad"}',
       h.resultTemplate([
        {"a":"2208"}, {"a":"2208"}
       ]), 200000);
	   
	   
	   	h.testCase("Merging entities 2208 and 3309", 'LINK FINAL entity:2208 entity:3309',
       ({status: true, errorCode: 0, info:'', result: ({type: 'text', data:'Entities 2208 and 3309 merged successfully.'})}), 200000);

	   h.testCase("Retrieving the entity which has the surname set to Abaitua Allende-Salazar after merging", 'SELECT $a WHERE { $a <surname> "Abaitua Allende-Salazar"}',
       h.resultTemplate([
        {"a":"2208"}, {"a":"3198"}
       ]), 200000);
	   
	   h.testCase("Retrieving the entity which has the surname set to Abad after merging", 'SELECT $a WHERE { $a <surname> "Abad"}',
       h.resultTemplate([
        {"a":"2208"}, {"a":"2208"}
       ]), 200000);
	   
	   h.testCase("Retrieving the property-value pairs of entity 3309 after merging entities 2208 and 3309", 'SELECT $a $b WHERE { entity:3309 $a $b}',
       h.resultTemplate([

       ]), 200000);
	   
	   h.testCase("Retrieving the property-value pairs of entity 3309 after merging entities 2208 and 3309", 'SELECT $a $b WHERE { entity:2208 $a $b}',
       h.resultTemplate([
            {"a":"type", b:"person"}, {"a":"forename", b:"Mariano"}, {"a":"forename", b:"Mariano"}, {"a":"forename", b:"Felipe"}, {"a":"forename", b:"Felipe Maria Castor Julian"}, {"a":"forename", b:"Felipe"}, {"a":"birthdate", b:"31/12/0031"}, {"a":"birthdate", b:"01/01/0001"}, {"a":"birthdate", b:"01/01/0001"}, {"a":"birthdate", b:"01/01/0001"}, {"a":"sex", b:"m"}, {"a":"sex", b:"m"}, {"a":"sex", b:"m"}, {"a":"sex", b:"m"}, {"a":"surname", b:"Abad"}, {"a":"surname", b:"Abad"}, {"a":"surname", b:"Abaitua"}, {"a":"surname", b:"Abaitua Allende-Salazar"}, {"a":"surname", b:"Abaitua"}, {"a":"birthplace", b:"1803"}, {"a":"birthplace", b:"1816"}
       ]), 200000);
	   
	   h.testCase("Testing meta property", 'SELECT $a $c WHERE { META $b { entity:2208 <surname> $c} . $b <fuz:source> $a }',
       h.resultTemplate([
        {"a":"0", "c":"Abad"}, {"a":"0", "c":"Abad"}, {"a":"0", "c":"Abaitua"}, {"a":"0", "c":"Abaitua Allende-Salazar"}, {"a":"0", "c":"Abaitua"}
       ]), 200000); 
	});		
	  
  });
}); 
