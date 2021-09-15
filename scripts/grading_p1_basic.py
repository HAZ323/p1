#!/usr/bin/python3
import cse303

# Configure constants and users
cse303.indentation = 80
cse303.verbose = cse303.check_args_verbose()
alice = cse303.UserConfig("alice", "alice_is_awesome")
fakealice = cse303.UserConfig("alice", "not_alice_password")
bob = cse303.UserConfig("bob", "bob_is_the_best")
chris = cse303.UserConfig("chris", "i_heart_cats")
diana = cse303.UserConfig("diana", "p@S$$$$sw0rd")
afilet1 = "server/responses.h"
afileb1 = "obj64/file.o"
afilet2 = "client/client.cc"
afileb2 = "obj64/err.o"
bfilet1 = "server/server.cc"
bfileb1 = "obj64/my_crypto.o"
bfilet2 = "client/requests.cc"
bfileb2 = "obj64/net.o"
allfile = "allfile"
makefiles = ["Makefile", "p1.aes.mk", "p1.rsa.mk", "p1.nocrypt.mk", "p1.file.mk"]

# Create objects with server and client configuration
server = cse303.ServerConfig("./obj64/server.exe", "9999", "rsa", "company.dir")
client = cse303.ClientConfig("./obj64/client.exe", "localhost", "9999", "localhost.pub")

# Check if we should use solution server and/or client
cse303.override_exe(server, client)

# Set up a clean slate before getting started
cse303.line()
print("Getting ready to run tests")
cse303.line()
cse303.makeclean() # make clean
cse303.clean_common_files(server, client) # .pub, .pri, .dir files
cse303.killprocs()
cse303.build(makefiles)
cse303.leftmsg("Copying files with solution aes/rsa into place")
cse303.copyexefile("obj64/server.p1.nocrypt.exe", "obj64/server.exe")
cse303.copyexefile("obj64/client.p1.nocrypt.exe", "obj64/client.exe")
cse303.okmsg()

# Basic functionality test #1: the REG command (and a little bit of BYE)
print()
cse303.line()
print("Test #1: Basic REG functionality")
cse303.line()
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Generating RSA keys as ("+server.keyfile+".pub, "+server.keyfile+".pri)",
    "File not found: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Registering to an empty file", "___OK___", client.reg(alice), server)
cse303.after(server.pid) # need an extra cleanup to handle the KEY that was sent by first REG
cse303.do_cmd("Registering to a non-empty file", "___OK___", client.reg(bob), server)
cse303.do_cmd("Re-registering with same password fails", "ERR_USER_EXISTS", client.reg(alice), server)
cse303.do_cmd("Re-registering with different password fails", "ERR_USER_EXISTS", client.reg(fakealice), server)
cse303.do_cmd("Using BYE to verify integrity of password", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
cse303.check_exist(server.dirfile, False)
cse303.clean_common_files(server, client)

# Basic functionality test #2: BYE requires authentication
print()
cse303.line()
print("Test #2: Authentication and BYE")
cse303.line()
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Generating RSA keys as ("+server.keyfile+".pub, "+server.keyfile+".pri)",
    "File not found: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Registering a user", "___OK___", client.reg(alice), server)
cse303.after(server.pid) # need an extra cleanup to handle the KEY that was sent by first REG
cse303.do_cmd("Invalid user for BYE", "ERR_LOGIN", client.bye(bob), server)
cse303.do_cmd("Registering another user", "___OK___", client.reg(bob), server)
cse303.do_cmd("Invalid password for BYE", "ERR_LOGIN", client.bye(fakealice), server)
cse303.do_cmd("Valid (but not first) user calls BYE", "___OK___", client.bye(bob), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
cse303.check_exist(server.dirfile, False)
cse303.clean_common_files(server, client)

# Basic functionality test #3: SET/GET
print()
cse303.line()
print("Test #3: SET and GET")
cse303.line()
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Generating RSA keys as ("+server.keyfile+".pub, "+server.keyfile+".pri)",
    "File not found: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Registering a user", "___OK___", client.reg(alice), server)
cse303.after(server.pid) # need an extra cleanup to handle the KEY that was sent by first REG
cse303.do_cmd("Setting alice's content (text).", "___OK___", client.setC(alice, afilet1), server)
cse303.do_cmd("Checking alice's content.", "___OK___", client.getC(alice, alice.name), server)
cse303.check_file_result(afilet1, alice.name)
cse303.do_cmd("Getting alice's content with bad user.", "ERR_LOGIN", client.getC(bob, alice.name), server)
cse303.do_cmd("Getting alice's content with pad password.", "ERR_LOGIN", client.getC(fakealice, alice.name), server)
cse303.do_cmd("Setting bob's content before registering.", "ERR_LOGIN", client.setC(bob, afilet1), server)

cse303.do_cmd("Overwriting alice's content (text).", "___OK___", client.setC(alice, afilet2), server)
cse303.do_cmd("Checking alice's content.", "___OK___", client.getC(alice, alice.name), server)
cse303.check_file_result(afilet2, alice.name)

cse303.do_cmd("Registering another user", "___OK___", client.reg(bob), server)
cse303.do_cmd("Setting bob's content (binary).", "___OK___", client.setC(bob, bfileb1), server)
cse303.do_cmd("Overwriting bob's content (binary).", "___OK___", client.setC(bob, bfileb2), server)
cse303.do_cmd("Checking bob's content.", "___OK___", client.getC(bob, bob.name), server)
cse303.check_file_result(bfileb2, bob.name)

cse303.do_cmd("Getting alice's content with bob.", "___OK___", client.getC(bob, alice.name), server)
cse303.check_file_result(afilet2, alice.name)
cse303.do_cmd("Getting bob's content with alice.", "___OK___", client.getC(alice, bob.name), server)
cse303.check_file_result(bfileb2, bob.name)

cse303.build_file("toobig.dat", 1048577)
cse303.do_cmd("Setting alice's content with too large file.", "ERR_REQ_FMT", client.setC(alice, "toobig.dat"), server)
cse303.delfile("toobig.dat")

cse303.build_file("nottoobig.dat", 1048576)
cse303.do_cmd("Setting alice's with large file.", "___OK___", client.setC(alice, "nottoobig.dat"), server)
cse303.do_cmd("Getting alice's content.", "___OK___", client.getC(bob, alice.name), server)
cse303.check_file_result("nottoobig.dat", alice.name)
cse303.delfile("nottoobig.dat")


cse303.do_cmd("Shutting down", "___OK___", client.bye(bob), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
cse303.check_exist(server.dirfile, False)
cse303.clean_common_files(server, client)

# Basic functionality test #4: ALL
print()
cse303.line()
print("Test #4: ALL")
cse303.line()
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Generating RSA keys as ("+server.keyfile+".pub, "+server.keyfile+".pri)",
    "File not found: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Registering a user", "___OK___", client.reg(alice), server)
cse303.after(server.pid) # need an extra cleanup to handle the KEY that was sent by first REG
cse303.do_cmd("Getting all users to make sure it's just alice.", "___OK___", client.getA(alice, allfile), server)
cse303.check_file_list(allfile, [alice.name])
cse303.do_cmd("Running ALL with invalid user.", "ERR_LOGIN", client.getA(bob, allfile), server)

cse303.do_cmd("Registering a user", "___OK___", client.reg(bob), server)
cse303.do_cmd("Registering a user", "___OK___", client.reg(chris), server)
cse303.do_cmd("Registering a user", "___OK___", client.reg(diana), server)
cse303.do_cmd("Getting all users to verify newlines.", "___OK___", client.getA(alice, allfile), server)
cse303.check_file_list(allfile, [alice.name, bob.name, chris.name, diana.name])

cse303.do_cmd("Shutting down", "___OK___", client.bye(chris), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
cse303.check_exist(server.dirfile, False)
cse303.clean_common_files(server, client)

# Basic functionality test #5: SAV
print()
cse303.line()
print("Test #5: Proper authentication for SAV")
cse303.line()
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Generating RSA keys as ("+server.keyfile+".pub, "+server.keyfile+".pri)",
    "File not found: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Registering a user", "___OK___", client.reg(alice), server)
cse303.after(server.pid) # need an extra cleanup to handle the KEY that was sent by first REG
cse303.do_cmd("Running SAV with invalid user.", "ERR_LOGIN", client.persist(bob), server)
cse303.do_cmd("Running SAV with invalid password.", "ERR_LOGIN", client.persist(fakealice), server)
cse303.do_cmd("Running SAV with valid user.", "___OK___", client.persist(alice), server)
cse303.do_cmd("Shutting down", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
cse303.check_exist(server.dirfile, True)
cse303.clean_common_files(server, client)

print()
