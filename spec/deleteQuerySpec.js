var net = require('net');
var h = require('./support/helper.js');

describe("fzdb", function() {
    beforeEach(function(done) {
        h.setupClient();
        h.sendCmd(h.loginToAdminQuery).then(function(data) {
            expect(data.result.data).toEqual('Logged in successfully.');
            done(); 
        });
    });
    afterEach(function(done) {
        h.sendCmd('USER LOGOUT').then(function(data) {
            expect(data.result.data).toEqual('Logged out successfully.');
            done(); 
        });
    });
	describe("delete queries:", function() {
		describe("DB with one entity", function() {
            var entityId ;
            beforeEach(function(done) {
                h.sendCmd('FLUSH').then(function() {
                    h.sendCmd("INSERT DATA { $a <forename> \"Fred\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                        entityId = data.result.data.a;
                        done(); 
                    });
                });
            });

            it("Fred is in DB", function(done) {
                h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }").then(function(data) {
                    expect((data.result.data).length).toBe(1);
                    done();
                });          
            });

            describe("Deleting Fred from DB", function() {
                beforeEach(function(done) {
                    var stat;
                    h.sendCmd("DELETE WHERE { $a <forename> \"Fred\" }").then(function(data) {
                        expect(data.status).toBe(true);
                        stat=data.status;
                        done();
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

		describe("delete with variable ID" , function () {
		    var jonesId;
		    var smithId;
            beforeEach(function(done) {
                h.sendCmd('FLUSH').then(function(data) {
                    expect(data.status).toBe(true);
                    expect(data.result.data).toEqual("Database cleared.");
                    h.sendCmd("INSERT DATA { $a <forename> \"Fred\" . $a <surname> \"Jones\" . $a <profession> \"Bartender\"} WHERE { NEW($a,\"person\") }").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data.a).not.toBeNull();
                        jonesId = data.result.data.a;
                        h.sendCmd("INSERT DATA { $a <forename> \"Fred\" . $a <surname> \"Smith\" . $a <profession> \"Singer\"} WHERE { NEW($a,\"person\") }").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data.result.data.a).not.toBeNull();
                            smithId = data.result.data.a;
                            done();
                        });
                    });
                });
            });

            del=function(cmd, testName) {
                it("status check" , function(done) {
                    h.sendCmd(cmd).then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data).toEqual('Deleted 1 entities 0 properties and 0 objects ');
                        done();
                    });
                });

                describe(testName+' delete', function() {
                    beforeEach(function(done) {
                        h.sendCmd(cmd).then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data.result.data).toEqual('Deleted 1 entities 0 properties and 0 objects ');
                            done();
                        });
                    });
                    it("Jones is gone", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <surname> \"Jones\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[]})}));
                            done();
                        });      
                    });
                    it("Smith is here", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: smithId})]})}));
                            done();
                        });      
                    });
                    it("Assert clean up", function(done) {
                        h.sendCmd("SELECT $a $b WHERE { entity:"+jonesId+" $a $b }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[]})}));
                            done();
                        });      
                    });
                    it("Smith is still a singer", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <profession> \"Singer\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: smithId})]})}));
                            done();
                        });      
                    });
                });
            }

            oldMethod = "DELETE      WHERE { $a <forename> \"Fred\" . $a <surname> \"Jones\" }";
            newMethod = "DELETE {$a} WHERE { $a <forename> \"Fred\" . $a <surname> \"Jones\" }";

            del(newMethod,'newMethod');
            del(oldMethod,'oldMethod');
        });

		describe("DB with linked entities:", function() {
            var smithId;
            var fredId;
            var delSmithQ = "DELETE WHERE { $a <surname> \"Smith\" }";
            var delFredQ = "DELETE WHERE { $a <forename> \"Fred\" }";
            var delSmithQs = "DELETE {$a} WHERE { $a <surname> \"Smith\" }";
            var delFredQs = "DELETE {$a} WHERE { $a <forename> \"Fred\" }";
            beforeEach(function(done) {
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
                                done(); 
                            });
                        });
                    });
                });
            });
            afterEach(function(done) {
                h.sendCmd('FLUSH').then(function(data) {
                    expect(data.status).toBe(true);
                    expect(data.result.data).toEqual("Database cleared.");
                    done();
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
                it("Smith",function(done){
                    h.sendCmd(delSmithQ) .then(function(data) {
                        expect(data.status).toEqual(false);
                        expect(data.errorCode).toEqual(8);
                        expect(data.info).toEqual('This entity currently has linkage with another entity, unlink them first.');
                        done();
                    });      
                });
                it("Fred",function(done){
                    h.sendCmd(delFredQ) .then(function(data) {
                        expect(data.status).toEqual(false);
                        expect(data.errorCode).toEqual(8);
                        expect(data.info).toEqual('This entity currently has linkage with another entity, unlink them first.');
                        done();
                    });      
                });
                it("Smith",function(done){
                    h.sendCmd(delSmithQs) .then(function(data) {
                        expect(data.status).toEqual(false);
                        expect(data.errorCode).toEqual(8);
                        expect(data.info).toEqual('This entity currently has linkage with another entity, unlink them first.');
                        done();
                    });      
                });
                it("Fred",function(done){
                    h.sendCmd(delFredQs) .then(function(data) {
                        expect(data.status).toEqual(false);
                        expect(data.errorCode).toEqual(8);
                        expect(data.info).toEqual('This entity currently has linkage with another entity, unlink them first.');
                        done();
                    });      
                });
               describe("Deleting Fred with linkage with variable select:", function() {
                    beforeEach(function(done) {
                        h.sendCmd(delFredQs) .then(function(data) {
                            done();
                        });      
                    });
                    it("Fred is still here", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                    it("Smith is still here.", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                });
                describe("Deleting Fred with linkage with variable select:", function() {
                    beforeEach(function(done) {
                        h.sendCmd(delFredQs) .then(function(data) {
                            done();
                        });      
                    });
                    it("Fred is still here", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                    it("Smith is still here.", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                });
                describe("Deleting Fred with linkage:", function() {
                    beforeEach(function(done) {
                        h.sendCmd(delFredQ) .then(function(data) {
                            done();
                        });      
                    });
                    it("Fred is still here", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                    it("Smith is still here.", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                });
                describe("Deleting Smith with linkage:", function() {
                    beforeEach(function(done) {
                        h.sendCmd(delSmithQ) .then(function(data) {
                            done();
                        });      
                    });
                    it("Fred is still here", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <forename> \"Fred\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                    it("Smith is still here.", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <surname> \"Smith\" }") .then(function(data) {
                            expect(data).toEqual(({status: true, errorCode: 0, info:'',
                                result: ({type: 'fsparql', data:[({a: fredId})]})}));
                            done();
                        });      
                    });
                });

            });
            describe("Entities after unlinking ", function() {
                beforeEach(function(done) {
                    h.sendCmd("UNLINK entity:" + fredId + " entity:" + smithId) .then(function(data) {
                        expect(data.status).toBe(true);
                        done();
                    });
                });
                describe("Can delete", function() {
                    it("Fred",function(done){
                        h.sendCmd(delFredQ).then(function(data) {
                            expect(data.status).toEqual(true);
                            expect(data.errorCode).toEqual(0);
                            done();
                        });      
                    });

                    it("Smith",function(done){
                        h.sendCmd(delSmithQ).then(function(data) {
                            expect(data.status).toEqual(true);
                            expect(data.errorCode).toEqual(0);
                            done();
                        });      
                    });

                });
                describe("Deleting Fred", function() {
                    beforeEach(function(done) {
                        h.sendCmd(delFredQs).then(function(data) {
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
                describe("Deleting Smith:",function() {
                    beforeEach(function(done) {
                        h.sendCmd(delSmithQ).then(function(data) {
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
            });
		});
		describe("Entities with properties and values:", function() {
		    var moeId;
		    var reusId;
		    var poloId;
            beforeEach(function(done) {
                h.sendCmd('FLUSH').then(function(data) {
                    expect(data.status).toBe(true);
                    expect(data.result.data).toEqual("Database cleared.");
                    h.sendCmd("INSERT DATA { $a <forename> \"Marco\"; <surname> \"Reus\"; <age> \"28\"; <drinks> \"Water\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data.a).not.toBeNull();
                        reusId=data.result.data.a;
                        h.sendCmd("INSERT DATA { $a <forename> \"Moe\"; <surname> \"Szyslak\"; <age> \"34\"; <drinks> \"Water\"; <profession> \"Bartender\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data.result.data.a).not.toBeNull();
                            moeId=data.result.data.a;
                            h.sendCmd("INSERT DATA { $a <forename> \"Marco\"; <surname> \"Polo\"; <age> \"34\"; <drinks> \"Wine\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                                expect(data.status).toBe(true);
                                expect(data.result.data.a).not.toBeNull();
                                poloId=data.result.data.a;
                                done();
                            });
                        });
                    });
                });
            });
            describe("Sanity Checks",function() {
                it("Entity numbers of forename Macro", function(done) {
                    h.sendCmd("SELECT $a WHERE { $a <forename> \"Marco\"}").then(function(data) {
                        expect(data).toEqual(({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'}), ({a: '4'})]})}));
                        done();
                    });
                });
                it("Entity number of Macro Reus", function(done) {
                    h.sendCmd("SELECT $a WHERE { $a <forename> \"Marco\" . $a <surname> \"Reus\"}").then(function(data) {
                    expect(data).toEqual(
                        ({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'})]})}));
                        done();
                    });
                });
                it("Ability to get <forename> from BGP", function(done) {
                    h.sendCmd("SELECT $a WHERE { entity:2 $a \"Marco\"}").then(function(data) {
                        expect(data.result.data).not.toBeNull();
                        expect(data).toEqual(h.resultTemplate([{a:'forename'}]));
                        done();
                    });
                });
                it("Entity numbers of drinks water", function(done) {
                    h.sendCmd("SELECT $a WHERE { $a <drinks> \"Water\"}").then(function(data) {
                        expect(data).toEqual(h.resultTemplate([{a:'2'},{a:'3'}]));
                        done();
                    });
                });
            });

            describe("Deleting values with irrelevant variable select", function() {
                it("existence before deleting", function(done) {
                    h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <profession> $o}").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(Object.keys(data.result.data).length).toBe(1);
                        expect(data).toEqual(h.resultTemplate([{o:'Bartender'}]));
                        done();
                    });
                });
                it("Deletes fine",function(done) {
                    h.sendCmd("DELETE {$p} WHERE { entity:"+moeId+" <profession> $o}").then(function(data) {
                        expect(data.status).toBe(true);
                        done();
                    });
                });
                describe("Deleting moe profession bartender", function() {
                    beforeEach(function(done) {
                        h.sendCmd("DELETE {$p} WHERE{ entity:"+moeId+" <profession> $o}").then(function(data) {
                            expect(data.status).toBe(true);
                            done();
                        });
                    });

                    it("Moe is still bartender",function(done) {
                        h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <profession> $o}").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(Object.keys(data.result.data).length).toBe(1);
                            done();
                        });
                    });

                    it("Moe forename is still Moe",function(done) {
                        h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <forename> $o}").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data).toEqual(h.resultTemplate([{o:'Moe'}]));
                            done();
                        });
                    });

                });
            });
            describe("Deleting values with variable select", function() {
                it("existence before deleting", function(done) {
                    h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <profession> $o}").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(Object.keys(data.result.data).length).toBe(1);
                        expect(data).toEqual(h.resultTemplate([{o:'Bartender'}]));
                        done();
                    });
                });
                it("Deletes fine",function(done) {
                    h.sendCmd("DELETE {$o} WHERE { entity:"+moeId+" <profession> $o}").then(function(data) {
                        expect(data.status).toBe(true);
                        done();
                    });
                });
                describe("Deleting moe profession bartender", function() {
                    beforeEach(function(done) {
                        h.sendCmd("DELETE {$o} WHERE{ entity:"+moeId+" <profession> $o}").then(function(data) {
                            expect(data.status).toBe(true);
                            done();
                        });
                    });

                    it("Moe is no longer bartender",function(done) {
                        h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <profession> $o}").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data).toEqual(h.resultTemplate([]));
                            done();
                        });
                    });

                    it("Moe forename is still Moe",function(done) {
                        h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <forename> $o}").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data).toEqual(h.resultTemplate([{o:'Moe'}]));
                            done();
                        });
                    });

                });
            });
            describe("Deleting values", function() {
                it("existence before deleting", function(done) {
                    h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <profession> $o}").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(Object.keys(data.result.data).length).toBe(1);
                        expect(data).toEqual(h.resultTemplate([{o:'Bartender'}]));
                        done();
                    });
                });
                it("Deletes fine",function(done) {
                    h.sendCmd("DELETE WHERE { entity:"+moeId+" <profession> $o}").then(function(data) {
                        expect(data.status).toBe(true);
                        done();
                    });
                });
                describe("Deleting moe profession bartender", function() {
                    beforeEach(function(done) {
                        h.sendCmd("DELETE WHERE { entity:"+moeId+" <profession> $o}").then(function(data) {
                            expect(data.status).toBe(true);
                            done();
                        });
                    });

                    it("Moe is no longer bartender",function(done) {
                        h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <profession> $o}").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data).toEqual(h.resultTemplate([]));
                            done();
                        });
                    });

                    it("Moe forename is still Moe",function(done) {
                        h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <forename> $o}").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data).toEqual(h.resultTemplate([{o:'Moe'}]));
                            done();
                        });
                    });

                });
            });
            describe("Deleting properties with variable",function() {
                it("existence before deleting", function(done) {
                    h.sendCmd("SELECT $p WHERE { entity:"+moeId+" $p \"Bartender\"}").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(Object.keys(data.result.data).length).toBe(1);
                        done();
                    });
                });
                it("Other properties still exist OK", function(done) {
                    h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <forename> $o}").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data).toEqual(h.resultTemplate([{o:'Moe'}]));
                        done();
                    });
                });
                it("no error when deleting profession",function(done) {
                    h.sendCmd("DELETE {$p} WHERE { entity:"+moeId+" $p \"Bartender\"}").then(function(data) {
                        expect(data.status).toBe(true);
                        done();
                    });
                });
                it("no error when deleting forename",function(done) {
                    h.sendCmd("DELETE {$p} WHERE { entity:"+moeId+" $p \"Moe\"}").then(function(data) {
                        expect(data.status).toBe(true);
                        done();
                    });
                });
                describe("that globally has only one name", function() {
                    beforeEach(function(done) {
                        h.sendCmd("DELETE {$p} WHERE { entity:"+moeId+" $p \"Bartender\"}").then(function(data) {
                            expect(data.status).toBe(true);
                            done();
                        });
                    });
                    it("return nothing when selected", function(done) {
                        h.sendCmd("SELECT $p WHERE { entity:"+moeId+" $p \"Bartender\"}").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data).toEqual(h.resultTemplate([]));
                            done();
                        });
                    });
                    it("Other properties still exist OK", function(done) {
                        h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <forename> $o}").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data).toEqual(h.resultTemplate([{o:'Moe'}]));
                            done();
                        });
                    });
                    it("Adding back does not break the system", function(done) {
                        h.sendCmd("INSERT DATA { entity:"+moeId+" <profession> \"Bartender\"}").then(function(data) {
                            expect(data.status).toBe(true);
                            done();
                        });
                    });
                });
                it("that globally has more than one name", function(done) {
                    h.sendCmd("SELECT $a WHERE { $a <forename> \"Marco\" . $a <surname> \"Reus\"}").then(function(data) {
                    expect(data).toEqual(
                        ({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'})]})}));
                        done();
                    });
                });
                it("after the only property got deleted, adding back, and query works fine.", function(done) {
                    h.sendCmd("SELECT $a WHERE { entity:2 $a \"Marco\"}").then(function(data) {
                        expect(data.result.data).not.toBeNull();
                        expect(data).toEqual(h.resultTemplate([{a:'forename'}]));
                        done();
                    });
                });
                it("Entity numbers of drinks water", function(done) {
                    h.sendCmd("SELECT $a WHERE { $a <drinks> \"Water\"}").then(function(data) {
                        expect(data).toEqual(h.resultTemplate([{a:'2'},{a:'3'}]));
                        done();
                    });
                });
                describe("deleting <forename> at Szyslak ", function() {
                    //Deleting forename of one deletes all others
                    beforeEach(function(done) {
                        h.sendCmd("DELETE {$p} WHERE { entity:"+moeId+" $p \"Moe\"}").then(function(data) {
                            expect(data.status).toBe(true);
                            done();
                        });
                    });

                    it("Szyslak is no longer moe", function(done) {
                        h.sendCmd("SELECT $s WHERE { entity:"+moeId+" <forename> \"Moe\"}").then(function(data) {
                            expect(data).toEqual(h.resultTemplate([]));
                            done();
                        });
                    });
                    it("Reus is no longer Macro", function(done) {
                        h.sendCmd("SELECT $s WHERE { entity:"+reusId+" <forename> \"Macro\"}").then(function(data) {
                            expect(data).toEqual(h.resultTemplate([]));
                            done();
                        });
                    });
                    it("Polo is no longer Macro", function(done) {
                        h.sendCmd("SELECT $s WHERE { entity:"+poloId+" <forename> \"Macro\"}").then(function(data) {
                            expect(data).toEqual(h.resultTemplate([]));
                            done();
                        });
                    });
                    it("Szyslak profession is still Bartender", function(done) {
                        h.sendCmd("SELECT $s WHERE { entity:"+moeId+" <profession> $s}").then(function(data) {
                            expect(data).toEqual(h.resultTemplate([({s:'Bartender'})]));
                            done();
                        });
                    });
                    it("Reus still drinks water", function(done) {
                        h.sendCmd("SELECT $s WHERE { entity:"+reusId+" <drinks> $s}").then(function(data) {
                            expect(data).toEqual(h.resultTemplate([({s:'Water'})]));
                            done();
                        });
                    });
                });
            });
            describe("Deleting properties",function() {
                it("existence before deleting", function(done) {
                    h.sendCmd("SELECT $p WHERE { entity:"+moeId+" $p \"Bartender\"}").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(Object.keys(data.result.data).length).toBe(1);
                        done();
                    });
                });
                it("Other properties still exist OK", function(done) {
                    h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <forename> $o}").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data).toEqual(h.resultTemplate([{o:'Moe'}]));
                        done();
                    });
                });
                it("no error when deleting profession",function(done) {
                    h.sendCmd("DELETE WHERE { entity:"+moeId+" $p \"Bartender\"}").then(function(data) {
                        expect(data.status).toBe(true);
                        done();
                    });
                });
                it("no error when deleting forename",function(done) {
                    h.sendCmd("DELETE WHERE { entity:"+moeId+" $p \"Moe\"}").then(function(data) {
                        expect(data.status).toBe(true);
                        done();
                    });
                });
                describe("that globally has only one name", function() {
                    beforeEach(function(done) {
                        h.sendCmd("DELETE WHERE { entity:"+moeId+" $p \"Bartender\"}").then(function(data) {
                            expect(data.status).toBe(true);
                            done();
                        });
                    });
                    it("return nothing when selected", function(done) {
                        h.sendCmd("SELECT $p WHERE { entity:"+moeId+" $p \"Bartender\"}").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data).toEqual(h.resultTemplate([]));
                            done();
                        });
                    });
                    it("Other properties still exist OK", function(done) {
                        h.sendCmd("SELECT $o WHERE { entity:"+moeId+" <forename> $o}").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data).toEqual(h.resultTemplate([{o:'Moe'}]));
                            done();
                        });
                    });
                    it("Adding back does not break the system", function(done) {
                        h.sendCmd("INSERT DATA { entity:"+moeId+" <profession> \"Bartender\"}").then(function(data) {
                            expect(data.status).toBe(true);
                            done();
                        });
                    });
                });
                it("that globally has more than one name", function(done) {
                    h.sendCmd("SELECT $a WHERE { $a <forename> \"Marco\" . $a <surname> \"Reus\"}").then(function(data) {
                    expect(data).toEqual(
                        ({status: true, errorCode: 0, info:'', result: ({type: 'fsparql', data:[({ a: '2'})]})}));
                        done();
                    });
                });
                it("after the only property got deleted, adding back, and query works fine.", function(done) {
                    h.sendCmd("SELECT $a WHERE { entity:2 $a \"Marco\"}").then(function(data) {
                        expect(data.result.data).not.toBeNull();
                        expect(data).toEqual(h.resultTemplate([{a:'forename'}]));
                        done();
                    });
                });
                it("Entity numbers of drinks water", function(done) {
                    h.sendCmd("SELECT $a WHERE { $a <drinks> \"Water\"}").then(function(data) {
                        expect(data).toEqual(h.resultTemplate([{a:'2'},{a:'3'}]));
                        done();
                    });
                });
                describe("deleting <forename> at Szyslak ", function() {
                    //Deleting forename of one deletes all others
                    beforeEach(function(done) {
                        h.sendCmd("DELETE WHERE { entity:"+moeId+" $p \"Moe\"}").then(function(data) {
                            expect(data.status).toBe(true);
                            done();
                        });
                    });

                    it("Szyslak is no longer moe", function(done) {
                        h.sendCmd("SELECT $s WHERE { entity:"+moeId+" <forename> \"Moe\"}").then(function(data) {
                            expect(data).toEqual(h.resultTemplate([]));
                            done();
                        });
                    });
                    it("Reus is no longer Macro", function(done) {
                        h.sendCmd("SELECT $s WHERE { entity:"+reusId+" <forename> \"Macro\"}").then(function(data) {
                            expect(data).toEqual(h.resultTemplate([]));
                            done();
                        });
                    });
                    it("Polo is no longer Macro", function(done) {
                        h.sendCmd("SELECT $s WHERE { entity:"+poloId+" <forename> \"Macro\"}").then(function(data) {
                            expect(data).toEqual(h.resultTemplate([]));
                            done();
                        });
                    });
                    it("Szyslak profession is still Bartender", function(done) {
                        h.sendCmd("SELECT $s WHERE { entity:"+moeId+" <profession> $s}").then(function(data) {
                            expect(data).toEqual(h.resultTemplate([({s:'Bartender'})]));
                            done();
                        });
                    });
                    it("Reus still drinks water", function(done) {
                        h.sendCmd("SELECT $s WHERE { entity:"+reusId+" <drinks> $s}").then(function(data) {
                            expect(data).toEqual(h.resultTemplate([({s:'Water'})]));
                            done();
                        });
                    });
                });
            });
		});
    });
});
