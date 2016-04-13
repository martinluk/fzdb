var net = require('net');
var h = require('./support/helper.js');

describe("fzdb", function() {
  var client;


	describe("delete queries:", function() {
		describe("DB with one entity", function() {
            var entityId ;
            beforeAll(function(done) {
                h.setupClient();
                h.sendCmd(h.loginToAdminQuery).then(function(data) {
                    expect(data.result.data).toEqual('Logged in successfully.');
                    h.sendCmd('FLUSH').then(function() {
                        h.sendCmd("INSERT DATA { $a <forename> \"Fred\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                            entityId = data.result.data.a;
                            h.sendCmd('USER LOGOUT').then(function(data) {
                                done(); 
                            });
                        });
                    });
                });
            });
            describe("deleting with correct variable", function() {
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
		});

		describe("DB with linked entities:", function() {
            var smithId;
            var fredId;
            beforeAll(function(done) {
                h.setupClient();
                h.sendCmd(h.loginToAdminQuery).then(function(data) {
                    expect(data.result.data).toEqual('Logged in successfully.');
                    h.sendCmd('FLUSH').then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data).toEqual("Database cleared.");
                        h.sendCmd("INSERT DATA { $a <forename> \"Fred\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data.result.data.a).not.toBeNull();
                            fredId = data.result.data.a;
                            h.sendCmd("INSERT DATA { $a <surname> \"Smith\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                                expect(data.status).toBe(true);
                                expect(data.result.data.a).not.toBeNull();
                                smithId = data.result.data.a;
                                h.sendCmd("LINK entity:" + fredId + " entity:" + smithId).then(function(data) {
                                    expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Entities ' + fredId + ' and ' + smithId + ' linked successfully.'})}));
                                    h.sendCmd('USER LOGOUT').then(function(data) {
                                        expect(data.status).toBe(true);
                                        done(); 
                                    });
                                });
                            });
                        });
                    });
                });
            });
            describe("Sanity check after linkage.", function() {
                it("getting the forename of entity:1 after link", function(done) {
                    h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }") .then(function(data) {
                        expect(data).toEqual(({status: true, errorCode: 0, info:'',
                            result: ({type: 'fsparql', data:[({a: fredId})]})}));
                        done();
                    });      
                });
                it("getting the forename of entity:1 after link", function(done) {
                    h.sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }") .then(function(data) {
                        expect(data).toEqual(({status: true, errorCode: 0, info:'',
                            result: ({type: 'fsparql', data:[({a: fredId})]})}));
                        done();
                    });      
                });
            });
            describe("Cannot delete linked entity", function() {
                beforeEach(function(done) {
                    h.sendCmd(h.loginToAdminQuery).then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data).toEqual('Logged in successfully.');
                        done();
                    });
                });
                afterEach(function(done) {
                    h.sendCmd('USER LOGOUT').then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data).toEqual('Logged out successfully.');
                        done();
                    });
                });
                it("debug check - asserting is admin.",function(done) {
                    h.sendCmd("USER LEVEL").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data).not.toBeNull();
                        expect(data.result.data).toBe('ADMIN');
                        done();
                    });
                });
                it("Smith",function(done){
                    h.sendCmd("DELETE $a WHERE { $a <surname> \"Smith\" }") .then(function(data) {
                        expect(data.status).toEqual(false);
                        expect(data.errorCode).toEqual(8);
                        expect(data.info).toEqual('This entity currently has linkage with another entity, unlink them first.');
                        done();
                    });      
                });
                it(", and both Smith and Fred is still here.", function() {
                    it("getting the forename of entity:1 after link", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                    it("getting the forename of entity:1 after link", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                });
                it("Fred",function(done){
                    h.sendCmd("DELETE $a WHERE { $a <forename> \"Fred\" }") .then(function(data) {
                        expect(data.status).toEqual(false);
                        expect(data.errorCode).toEqual(8);
                        expect(data.info).toEqual('This entity currently has linkage with another entity, unlink them first.');
                        done();
                    });      
                });
                it(", and both Smith and Fred is still here.", function() {
                    it("getting the forename of entity:1 after link", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                    it("getting the forename of entity:1 after link", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                });

            });
            describe("Entities after unlinking", function() {
                beforeAll(function(done) {
                    h.sendCmd(h.loginToAdminQuery).then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data).toEqual('Logged in successfully.');
                        h.sendCmd("UNLINK entity:" + fredId + " entity:" + smithId) .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'text', data: 'Entities ' + fredId + ' and ' + smithId + ' unlinked successfully.'})}));
                            done();
                        });     
                    });
                });
                afterAll(function(done) {
                    h.sendCmd('USER LOGOUT').then(function(data) {
                        if(!data.status) {
                            console.log(data);
                        }
                        expect(data.status).toBe(true);
                        expect(data.result.data).toEqual('Logged out successfully.');
                        done();
                    });
                });
                it("Smith", function() {
                    it("Deleting",function(done){
                        h.sendCmd("DELETE $a WHERE { $a <surname> \"Smith\" }").then(function(data) {
                            expect(data.status).toEqual(true);
                            expect(data.errorCode).toEqual(0);
                            done();
                        });      
                    });
                    it("Fred still here", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                    it("Smith is gone.", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[]})}));
                            done();
                        });      
                    });
                });
                it("Fred", function() {
                    it("Deleting",function(done){
                        h.sendCmd("DELETE $a WHERE { $a <forename> \"Smith\" }").then(function(data) {
                            expect(data.status).toEqual(true);
                            expect(data.errorCode).toEqual(0);
                            done();
                        });      
                    });
                    it("Fred is gone", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[]})}));
                            done();
                        });      
                    });
                    it("Smith is still here.", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: smithId})]})}));
                            done();
                        });      
                    });
                });

            });
		});
		describe("Entities with properties:", function() {
		    pending("deleting entities with properties not implemented yet");
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
