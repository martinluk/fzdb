var net = require('net');
var h = require('./support/helper.js');

//TODO I guess we can do serializing and deserializing too....
fdescribe("fzdb - unicode support", function() {
    beforeEach(function(done) {
        h.setupClient();
        h.sendCmd(h.loginToAdminQuery).then(function(data) {
            expect(data.result.data).toEqual('Logged in successfully.');
            h.sendCmd('FLUSH').then(function(data) {
                expect(data.status).toBe(true);
                done(); 
            });
        });
    });
    afterEach(function(done) {
        h.sendCmd('USER LOGOUT').then(function(data) {
            expect(data.result.data).toEqual('Logged out successfully.');
            done(); 
        });
    });

    userTest=function(username,password) {
        describe("with username " + username + "and password "+password, function() {
            beforeEach(function(done) {
                h.sendCmd('USER ADD '+username+' '+password).then(function(data) {
                    expect(data.status).toBe(true);
                    done();
                });
            });
            afterEach(function(done) {
                h.sendCmd('USER LOGOUT').then(function(data) {
                    h.sendCmd(h.loginToAdminQuery).then(function(data) {
                        h.sendCmd('USER DELETE '+username).then(function(data) {
                            expect(data.status).toBe(true);
                            done();
                        });
                    });
                });
            });

            it("able to login", function(done) {
                h.sendCmd('USER LOGIN '+username +' '+password).then(function(data) {
                    expect(data.status).toBe(true);
                    done();
                });
            });
        });
    }

    languageTest=function(lang, word) {
        describe("Language: "+ lang + ", word=" + word +":",function() {
            it("simple value correctly into database",function(done) {
                h.sendCmd("INSERT DATA { $a <forename> \""+word+"\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                    expect(data.status).toBe(true);
                    expect(data.result.data).toEqual({a:'2'});
                    done();
                });
            });
            describe("after simple value added into database",function() {
                beforeEach(function(done) {
                    h.sendCmd("INSERT DATA { $a <forename> \""+word+"\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data).toEqual({a:'2'});
                        done();
                    });
                });
                it("retrivable correctly", function(done) {
                    h.sendCmd("SELECT $a WHERE { $a <forename> \""+word+"\" }").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data).toEqual([{a:'2'}]);
                        done();
                    });
                });
                it("deletes correctly", function(done) {
                    h.sendCmd("DELETE WHERE { $a <forename> \""+word+"\" }").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data).toEqual('Deleted 1 entities 0 properties and 0 objects ');
                        done();
                    });
                });
                describe("after deleting", function() {
                    beforeEach(function(done) {
                        h.sendCmd("DELETE WHERE { $a <forename> \""+word+"\" }").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data.result.data).toEqual('Deleted 1 entities 0 properties and 0 objects ');
                            done();
                        });
                    });
                    it("deletes correctly", function(done) {
                        h.sendCmd("SELECT $a WHERE { $a <forename> \""+word+"\" }").then(function(data) {
                            expect(data.status).toBe(true);
                            expect(data.result.data).toEqual([]);
                            done();
                        });
                    });
                });
            });
            it("simple property correctly into database",function(done) {
                h.sendCmd("INSERT DATA { $a <"+word+"> \"value\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                    expect(data.status).toBe(true);
                    expect(data.result.data).toEqual({a:'2'});
                    done();
                });
            });
            describe("after simple value added into database",function() {
                beforeEach(function(done) {
                    h.sendCmd("INSERT DATA { $a <"+word+"> \"value\" } WHERE { NEW($a,\"person\") }").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data).toEqual({a:'2'});
                        done();
                    });
                });
                it("retrivable correctly", function(done) {
                    h.sendCmd("SELECT $a WHERE { $a <"+word+"> \"value\" } ").then(function(data) {
                        expect(data.status).toBe(true);
                        expect(data.result.data).toEqual([{a:'2'}]);
                        done();
                    });
                });
                it("deletes correctly", function(done) {
                    h.sendCmd("DELETE WHERE { $a <"+word+"> \"value\" } ").then(function(data) {
                        expect(data.status).toBe(true);
                        done();
                    });
                });
            });

            userTest(word,'pw');
            userTest('name', word);

        });
    }

    describe("Commonly used language", function() {
        //http://www.cl.cam.ac.uk/~mgk25/ucs/examples/quickbrown.txt, and skipped out ascii words
        testData={
            'Danish'            : 'jordbær fløde på',
            'German'            : 'Üben quält größeren',
            'Greek'             : 'Ξεσκεπάζω τὴν ψυχοφθόρα βδελυγμία',
            'Spanish'           : 'pingüino kilómetros frío añoraba',
            'Hungarian'         : 'Árvíztűrő tükörfúrógép',
            'Japanese-Katakana' : 'イロハニホヘト チリヌルヲ',
            'Hebew'             : 'איך הקליטה',
            'Russian'           : 'Съешь же ещё этих мягких французских булок да выпей чаю',
            'Thai'              : 'จงฝ่าฟันพัฒนาวิชาการ กว่าบรรดาฝูงสัตว์เดรัจฉาน จ๋าๆ ฯ'
        }

        for (var lang in testData) {
            sentence = testData[lang];
            var words = sentence.split(' ');
            for(var word in words) {
                //For each spaced 'word', run the language test rig.
                languageTest(lang,words[word]);
            }
        }
    });
});
