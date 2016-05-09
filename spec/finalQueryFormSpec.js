var h = require('./support/helper.js');
var _ = require('lodash');

var simpsonsTestData = h.getData("simpsons_meta.fuz");

// load test data

describe("Fuzzy Database:finalQueryFormSpec", function() { 

  
  describe("sends the command over TCP with the simpsons data loaded", function() {

    beforeAll(function(done) {
        //Not pretty I know, will refactor later once everything is working,.
        h.setupClient();
        h.sendCmd(h.loginToAdminQuery).then(function() {
            h.sendCmd('FLUSH').then(function() {
                h.sendCmd(simpsonsTestData).then(function(){
                    h.sendCmd('USER LOGOUT').then(function() {done();});
            });
            });
        });
    });

      h.testCase("form VVU",  
        `SELECT $i $j $a WHERE { entity:3 <forename> $a . $i $j $a }`,
        h.resultTemplate(
          [{"i":"3","j":"forename", "a": "Marge"}]
        )
      );

      h.testCase("form VUV",  
        `SELECT $i $a $j WHERE { entity:3 $a "Marge" . $i $a $j }`,
        h.resultTemplate(
          [{"i":"2","a":"forename", "j": "Homer"},
          {"i":"3","a":"forename", "j": "Marge"},
          {"i":"4","a":"forename", "j": "Ned"},
          {"i":"5","a":"forename", "j": "Moe"},
          {"i":"2","a":"forename", "j": "Max"}]
        )
      );

      h.testCase("form UVV",  
        `SELECT $a $i $j WHERE { $a <forename> "Marge" . $a $i $j }`,
        h.resultTemplate(
          [{"a":"3","i":"type", "j": "person"},
          {"a":"3","i":"forename", "j": "Marge"},
          {"a":"3","i":"surname", "j": "Simpson"},
          {"a":"3","i":"age", "j": "34"},
          {"a":"3","i":"surname", "j": "Bouvier"}]
        )
      );

      h.testCase("form UUV",  
        `SELECT $a $b $i WHERE { $a $b "Homer" . $a $b $i }`,
        h.resultTemplate(
          [{"a":"2","b":"forename", "i": "Homer"},
          {"a":"2","b":"forename", "i": "Max"}]
        )
      );

      // h.xtestCase("form VUU",  
      //   `SELECT $i $a $b WHERE { entity:2 $a $b . $i $a $b }`,
      //   h.resultTemplate(
      //     [{"a":"2","b":"forename", "i": "Homer"},
      //     {"a":"2","b":"forename", "i": "Max"}]
      //   )
      // );

      // h.xtestCase("form UVU",  
      //   `SELECT $a $i $b WHERE { $a <forename> $b . $a $i $b }`,
      //   h.resultTemplate(
      //     [{"a":"2","b":"forename", "i": "Homer"},
      //     {"a":"2","b":"forename", "i": "Max"}]
      //   )
      // );

      // h.xtestCase("form UUV 2",  
      //   `SELECT $a $b $i $cert WHERE { META $m { $a $b "Homer" . $a $b $i } . $m <fuz:certainty> $cert }`,
      //   h.resultTemplate(
      //     [{"a":"2","b":"forename", "i": "Homer"},
      //     {"a":"2","b":"forename", "i": "Max"}]
      //   )
      // );

    });
 
});
