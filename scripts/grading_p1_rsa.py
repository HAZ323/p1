#!/usr/bin/python3
import cse303

# Configure constants and users
cse303.indentation = 80
cse303.verbose = cse303.check_args_verbose()
alice = cse303.UserConfig("alice", "alice_is_awesome")
fakealice = cse303.UserConfig("alice", "not_alice_password")
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
cse303.leftmsg("Copying RSA files into place")
cse303.copyexefile("obj64/server.p1.rsa.exe", "obj64/server.exe")
cse303.copyexefile("obj64/client.p1.rsa.exe", "obj64/client.exe")
cse303.okmsg()

# The RSA test runs a battery of GET and SET, to stress crypto.o
print()
cse303.line()
print("Test: RSA: Stressing RSA (be sure to run script with SERVER, CLIENT, and no overrides)")
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

cse303.do_cmd("Overwriting alice's content (text).", "___OK___", client.setC(alice, afilet2), server)
cse303.do_cmd("Checking alice's content.", "___OK___", client.getC(alice, alice.name), server)
cse303.check_file_result(afilet2, alice.name)

cse303.do_cmd("Setting alice's content (binary).", "___OK___", client.setC(alice, bfileb1), server)
cse303.do_cmd("Checking alice's content.", "___OK___", client.getC(alice, alice.name), server)
cse303.check_file_result(bfileb1, alice.name)

cse303.do_cmd("Overwriting alice's content (binary).", "___OK___", client.setC(alice, bfileb2), server)
cse303.do_cmd("Checking alice's content.", "___OK___", client.getC(alice, alice.name), server)
cse303.check_file_result(bfileb2, alice.name)

cse303.do_cmd("Shutting down", "___OK___", client.bye(alice), server)
cse303.await_server("Waiting for server to shut down.", "Server terminated", server)
cse303.clean_common_files(server, client)

print()
