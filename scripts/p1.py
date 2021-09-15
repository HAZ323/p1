#!/usr/bin/python3
import cse303

# Configure constants and users
cse303.indentation = 80
cse303.verbose = cse303.check_args_verbose()
alice = cse303.UserConfig("alice", "alice_is_awesome")
fakealice = cse303.UserConfig("alice", "not_alice_password")
bob = cse303.UserConfig("bob", "bob_is_the_best")
afile1 = "server/responses.h"
afile2 = "obj64/file.o"
allfile = "allfile"

# Create objects with server and client configuration
server = cse303.ServerConfig("./obj64/server.exe", "9999", "rsa", "company.dir")
client = cse303.ClientConfig("./obj64/client.exe", "localhost", "9999", "localhost.pub")

# Check if we should use solution server or client
cse303.override_exe(server, client)

# Set up a clean slate before getting started
cse303.line()
print("Getting ready to run tests")
cse303.line()
cse303.clean_common_files(server, client) # .pub, .pri, .dir files
cse303.killprocs()

print()
cse303.line()
print("Test #1: Registering users, putting content, and persisting")
cse303.line()
server.pid = cse303.do_cmd_a("Starting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Generating RSA keys as ("+server.keyfile+".pub, "+server.keyfile+".pri)",
    "File not found: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Registering new user alice.", "___OK___", client.reg(alice), server)
cse303.after(server.pid) # need an extra cleanup to handle the KEY that was sent by first REG
cse303.do_cmd("Setting alice's content.", "___OK___", client.setC(alice, afile1), server)
cse303.do_cmd("Checking alice's content.", "___OK___", client.getC(alice, alice.name), server)
cse303.check_file_result(afile1, alice.name)
cse303.do_cmd("Getting all users to make sure it's just alice.", "___OK___", client.getA(alice, allfile), server)
cse303.check_file_list(allfile, [alice.name])
cse303.do_cmd("Instructing server to persist data.", "___OK___", client.persist(alice), server)
cse303.do_cmd("Stopping server.", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)

print()
cse303.line()
print("Test #2: Make sure persistence works, test other features")
cse303.line()
server.pid = cse303.do_cmd_a("Restarting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Loaded: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Re-registering alice.", "ERR_USER_EXISTS", client.reg(alice), server)
cse303.do_cmd("Checking alice's old content.", "___OK___", client.getC(alice, alice.name), server)
cse303.check_file_result(afile1, alice.name)
cse303.do_cmd("Attempting access with bad password.", "ERR_LOGIN", client.getC(fakealice, alice.name), server)
cse303.do_cmd("Attempting access with bad user.", "ERR_LOGIN", client.getC(bob, alice.name), server)
cse303.do_cmd("Registering user bob.", "___OK___", client.reg(bob), server)
cse303.do_cmd("Attempting to access alice's data by bob.", "___OK___", client.getC(bob, alice.name), server)
cse303.check_file_result(afile1, alice.name)
cse303.do_cmd("Getting bob's nonexistent data.", "ERR_NO_DATA", client.getC(bob, bob.name), server)
cse303.do_cmd("Getting all users to make sure it's alice and bob.", "___OK___", client.getA(alice, allfile), server)
cse303.check_file_list(allfile, [bob.name, alice.name])
cse303.do_cmd("Getting all users to make sure it's alice and bob.", "___OK___", client.getA(alice, allfile), server)
cse303.check_file_list(allfile, [alice.name, bob.name])
cse303.do_cmd("Instructing server to persist data.", "___OK___", client.persist(alice), server)
cse303.do_cmd("Stopping server.", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)

print()
cse303.line()
print("Test #3: Overwrites of user data")
cse303.line()
server.pid = cse303.do_cmd_a("Restarting server:", [
    "Listening on port "+server.port+" using (key/data) = (rsa, "+server.dirfile+")",
    "Loaded: " + server.dirfile], server.launchcmd())
cse303.waitfor(2)
cse303.do_cmd("Setting alice's content.", "___OK___", client.setC(alice, afile2), server)
cse303.do_cmd("Checking alice's content.", "___OK___", client.getC(alice, alice.name), server)
cse303.check_file_result(afile2, alice.name)
cse303.do_cmd("Instructing server to persist data.", "___OK___", client.persist(alice), server)
cse303.do_cmd("Stopping server.", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)

cse303.clean_common_files(server, client)

print()
