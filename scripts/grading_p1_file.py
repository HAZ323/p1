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
cse303.leftmsg("Copying persistence files into place")
cse303.copyexefile("obj64/server.p1.file.exe", "obj64/server.exe")
cse303.copyexefile("obj64/client.p1.file.exe", "obj64/client.exe")
cse303.okmsg()

# REG+SAV should have the right file size, and should reload
print()
cse303.line()
print("Test #1: REG gets saved correctly")
cse303.line()
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Generating RSA keys as ("+server.keyfile+".pub, "+server.keyfile+".pri)",
    "File not found: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Registering alice", "___OK___", client.reg(alice), server)
cse303.after(server.pid) # need an extra cleanup to handle the KEY that was sent by first REG
cse303.do_cmd("Persisting", "___OK___", client.persist(alice), server)
cse303.do_cmd("Stopping server", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
expect_size1 = cse303.next8(8 + 8 + len(alice.name) + 8 + 16 + 8 + 32 + 8)
cse303.verify_filesize(server.dirfile, expect_size1)
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Loaded: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Stopping server", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
expect_size1 = cse303.next8(8 + 8 + len(alice.name) + 8 + 16 + 8 + 32 + 8)
cse303.verify_filesize(server.dirfile, expect_size1)

# Register a few more users
print()
cse303.line()
print("Test #2: Several REGs result in a correct file")
cse303.line()
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Loaded: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Registering bob", "___OK___", client.reg(bob), server)
cse303.do_cmd("Registering chris", "___OK___", client.reg(chris), server)
cse303.do_cmd("Registering diana", "___OK___", client.reg(diana), server)
cse303.do_cmd("Persisting", "___OK___", client.persist(alice), server)
cse303.do_cmd("Stopping server", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Loaded: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Verifying bob with GET", "ERR_NO_DATA", client.getC(bob, bob.name), server)
cse303.do_cmd("Verifying chris with GET", "ERR_NO_DATA", client.getC(chris, chris.name), server)
cse303.do_cmd("Verifying diana with GET", "ERR_NO_DATA", client.getC(diana, diana.name), server)
cse303.do_cmd("Stopping server", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
expect_size2 = 0
for s in [alice, bob, chris, diana]:
    expect_size2 += cse303.next8(8 + 8 + len(s.name) + 8 + 16 + 8 + 32 + 8)
cse303.verify_filesize(server.dirfile, expect_size2)

# Some SETs and some overwriting SETs
print()
cse303.line()
print("Test #3: SET commands (binary and text) persist correctly")
cse303.line()
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Loaded: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Set alice content", "___OK___", client.setC(alice, afilet1), server)
cse303.do_cmd("Set bob content", "___OK___", client.setC(bob, bfilet1), server)
cse303.do_cmd("Overwrite bob content", "___OK___", client.setC(bob, bfileb1), server)
cse303.do_cmd("Set chris content", "___OK___", client.setC(chris, bfileb2), server)
cse303.do_cmd("Set diana content", "___OK___", client.setC(diana, afileb1), server)
cse303.do_cmd("Overwrite diana's content", "___OK___", client.setC(diana, afileb2), server)
cse303.do_cmd("Persisting", "___OK___", client.persist(alice), server)
cse303.do_cmd("Stopping server", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Loaded: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)

cse303.do_cmd("Checking alice's content.", "___OK___", client.getC(alice, alice.name), server)
cse303.check_file_result(afilet1, alice.name)
cse303.do_cmd("Checking bob's content.", "___OK___", client.getC(bob, bob.name), server)
cse303.check_file_result(bfileb1, bob.name)
cse303.do_cmd("Checking chris's content.", "___OK___", client.getC(chris, chris.name), server)
cse303.check_file_result(bfileb2, chris.name)
cse303.do_cmd("Checking diana's content.", "___OK___", client.getC(diana, diana.name), server)
cse303.check_file_result(afileb2, diana.name)

cse303.do_cmd("Stopping server", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
expect_size3 = 0
expect_size3 += cse303.next8(8 + 8 + len(alice.name) + 8 + 16 + 8 + 32 + 8 + cse303.get_len(afilet1))
expect_size3 += cse303.next8(8 + 8 + len(bob.name) + 8 + 16 + 8 + 32 + 8 + cse303.get_len(bfileb1))
expect_size3 += cse303.next8(8 + 8 + len(chris.name) + 8 + 16 + 8 + 32 + 8 + cse303.get_len(bfileb2))
expect_size3 += cse303.next8(8 + 8 + len(diana.name) + 8 + 16 + 8 + 32 + 8 + cse303.get_len(afileb2))
cse303.verify_filesize(server.dirfile, expect_size3)

# Make sure the file format matches the solution server
#
# NB: The student server just produced a file and then loaded it again.  If the
#     solution server can also load that file, then we know the format is
#     correct
server = cse303.ServerConfig("./solutions/server.exe", "9999", "rsa", "company.dir")
print()
cse303.line()
print("Test #4: File format is correct (uses solution server)")
cse303.line()
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Loaded: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Checking alice's content.", "___OK___", client.getC(alice, alice.name), server)
cse303.check_file_result(afilet1, alice.name)
cse303.do_cmd("Checking bob's content.", "___OK___", client.getC(bob, bob.name), server)
cse303.check_file_result(bfileb1, bob.name)
cse303.do_cmd("Checking chris's content.", "___OK___", client.getC(chris, chris.name), server)
cse303.check_file_result(bfileb2, chris.name)
cse303.do_cmd("Checking diana's content.", "___OK___", client.getC(diana, diana.name), server)
cse303.check_file_result(afileb2, diana.name)

cse303.do_cmd("Stopping server", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
cse303.verify_filesize(server.dirfile, expect_size3)

cse303.clean_common_files(server, client)

print()
