var net = require('net');
var h = require('./support/helper.js');

describe("fzdb", function() {
  var client;


	describe("delete queries:", function() {
		fdescribe("DB with one entity", function() {
            var entityId ;
            beforeAll(function(done) {
            //Not pretty I know, will refactor later once everything is working,.
                h.setupClient();
                h.sendCmd(h.loginToAdminQuery).then(function() {
                    h.sendCmd('FLUSH').then(function() {
                        h.sendCmd('USER LOGOUT').then(function() {
                            done();
                        });
                    });
                });
            });

			it("no one used to call Fred", function(done) {
				h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }").then(function(data) {
					expect((data.result.data).length).toBe(0); done();
				});          
			});
			it("Adding Fred into DB", function(done) {
			    h.sendCmd(h.loginToAdminQuery).then(function(data) {
                    expect(data.result.data).toEqual('Logged in successfully.');
                    h.sendCmd("INSERT DATA { $a <forename> \"Fred\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                        entityId = data.result.data.a;
                        console.log(entityId);
                        h.sendCmd('USER LOGOUT').then(function(data) {
                            done(); 
                        });
                    });
                });
			});
			it("and now Fred lives in DB", function(done) {
				h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }").then(function(data) {
					expect((data.result.data).length).toBe(1);
					done();
				});          
			});
			it("Running the delete query", function(done) {
			    var stat;
			    h.sendCmd(h.loginToAdminQuery).then(function(data) {
                    expect(data.result.data).toEqual('Logged in successfully.');
                    h.sendCmd("DELETE $a WHERE { $a <forename> \"Fred\" }").then(function(data) {
                        stat=data.status;
                        h.sendCmd('USER LOGOUT').then(function(data) {
                            expect(data.status).toBe(true);
                            expect(stat).toBe(true);
                            done(); 
                        });
                    });
                });
            });    
			it("Resulting in Fred no longer lives in DB", function(done) {
				h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }").then(function(data) {
					expect((data.result.data).length).toBe(0);
					done();
				});          
			});
		});

		xdescribe("DB with linked entities:", function() {
			/* XXX Need double checking behaviour
			 * Having entity 1 and entity 2 linked together
			 * Deleting entity 1
			 * Removes entity 1
			 * and does not remove entity 2
			 */
			it("Starting new db state", function(done) {
				sendCmd("FLUSH").then(function(data) {
					done(); 
				});    
			});
			it("'Entity 1 contains no data", function(done) {
				sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }").then(function(data) {
					expect((data.result.data).length).toBe(0);
					done();
				});          
			});    
			it("'Entity 2 contains no data", function(done) {
				sendCmd("SELECT $a WHERE { $a <forename> \"Smith\" }").then(function(data) {
					expect((data.result.data).length).toBe(0);
					done();
				});          
			});    
			it("Having entity 1", function(done) {
				sendCmd("INSERT DATA { entity:1 <forename> \"Fred\" }") .then(function(data) {done(); });    
			});    
			it("Having entity 2", function(done) {
				sendCmd("INSERT DATA { entity:2 <surname> \"Smith\" }") .then(function(data) { done(); });       
			});
			it("'Entity 2 contains data", function(done) {
				sendCmd("SELECT $a WHERE { $a <forename> \"Fred\"}").then(function(data) {
					//console.log(data);
					expect(data.result.data).not.toBe("Inserted 1 triples."); //Should not be the status code of last status?
					//TODO Assert contains entity 2
					done();
				});          
			});    
			it("and entity 1 and entity 2 linked together", function(done) {
				sendCmd("LINK entity:1 entity:2").then(function(data) {
					expect(data).toEqual(jasmine.objectContaining({
						status:true,
						errorCode: 0
					}));
					done();
				});     
			});
			it("Deleting entity 1", function(done) {
				sendCmd("DELETE DATA {  <forename> \"Fred\" }").then(function(data) {
					expect(data).toEqual(jasmine.objectContaining({
						status:true,
						errorCode: 0
					}));
				done();
				});
			});
			it("'Removes entity 1 from db", function(done) {
				sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }").then(function(data) {
					expect(data.errorCode).toBe(0);
					done();
				});          
			});    
			it("And does not remove entity 2 from db", function(done) {
				sendCmd("SELECT $a WHERE { $a <forename> \"Smith\" }").then(function(data) {
					expect((data.result.data).length).toBe(0);
					done();
				});          
			});    
			it("Final clean up", function(done) {
				sendCmd("FLUSH").then(function(data) { done(); });    
			});

		});
		describe("Entities with properties:", function() {
			/* XXX Need double checking behaviour
			 * Add with two properties
			 * Delete one of the property
			 * Left with undeleted property
			 * Delete remaining property
			 * No property left
			 */
			it("Starting new db state", function(done) {
				sendCmd("FLUSH").then(function(data) { done(); });    
			});

			it("setting entity:2's forename to 'Ned' and surname to 'Flanders'", function(done) {
				client.write("INSERT DATA { entity:2 <forename> \"Ned\"; <surname> \"Flanders\" }");
					client.once('data', function(data) {
					done();
				});      
			});
		});
    });
});
