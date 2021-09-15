# Assignment #1: Creating a Secure On-Line Service

The purpose of this assignment is to get you thinking about how to compose
different security concepts to create an on-line service with authentication and
encryption.

## Do This First

Immediately after pulling `p1` from Bitbucket, you should start a container,
navigate to the `p1` folder, and type `chmod +x solutions/*.exe`.  This command
only needs to be run once.  It will make sure that certain files from the
assignment have their executable bit set.

## Assignment Details

During the warm-up assignment, we reviewed many powerful concepts.  We saw how
to load a file from disk as a byte stream, and how to send byte streams to disk
as new files.  We saw how to use RSA and AES encryption.  We saw how to send
bytes over the network and how to receive them.  In this assignment, we are
going to start putting those ideas together to build an on-line service.

As with any service, we will have two separate programs: a client and a server.
For now, the server will only handle authentication.  The server will manage a
"directory" of users.  Each user can store up to 1MB of data as their "profile
file".

Our focus is on security, and we have two goals.  The first is to ensure that
users cannot make unauthorized accesses.  To that end, we will have a
registration mechanism by which users can get an account on the system.  A user
will have permission to read any other user's profile file, and to get a list of
all user names.  A user will also have permission to change her/his own profile
file.

The second security goal is to make sure that communications between the client
and server are secure.  Any intermediary who intercepts a transmission should
not be able to decipher it to figure out what is being communicated.  Note that
this is not "end-to-end encryption", because we are not storing encrypted data
on the server.  When you have finished this assignment, you should have a good
sense for what additional effort it would take to achieve end-to-end encryption.

The final requirement of the service is that it has a rudimentary form of
persistence, wherein the entire directory gets saved to disk in response to
specific client requests.  This is not a great long-term strategy, but it will
be sufficient for now.

Note that the server will not be concurrent yet.  It need only handle one client
at a time.

## Getting Started

Your git repository contains a significant portion of the code for the final
solution to this assignment.  The `p1/common/` folder has variants of many of
the useful functions that we created for assignment 0.  The `p1/client/` folder
has code that is specific to the client, and the `p1/server/` folder has code
that is specific to the server.  You should be able to type `make` from the
`p1/` folder to build all of the code, and you should be able to run the basic
tests for the assignment by typing the following from the `p1` folder:

```bash
python3 scripts/p1.py
```

Of course, none of the tests will pass yet.  It will print some errors, and then
the script will crash.

To start understanding the low-level details of how the client and server are
supposed to work, you should read the `common/protocol.h` file.  This file
documents the protocol for client/server interaction.  Note that the encryption
procedure we use is very carefully designed, and very easy to get wrong.  There
are two tricky parts:

* The first is that the client needs to put information into the right "blocks"
  of its message.  As you write your code, it will become apparent why we have
  defined the `rblock` as we have.  As a hint, you might want to consider how
  you would allocate a buffer for the `ablock` if you didn't know its size.
* The second is that AES encryption is very fickle.  When you AES-encrypt data,
  you must make sure to properly handle the last block.  You should not be using
  a file as a temporary storage place for the data you encrypt/decrypt.
  Consequently, you will need to redesign the AES encryption code from
  assignment 0.

## Tips and Reminders

The code that we provide makes use of a number of C++ features.  You should be
sure you understand the way we use `std::vector`.  In particular, you should
know the difference between a declaration like `std::vector<uint8_t> v(1024)`
and the two-instruction sequence `std::vector<uint8_t> v; v.reserve(1024)`.
Also, note that our use of `std::vector` can introduce unnecessary copying,
which should be optimized out in an advanced implementation.  On the other hand,
some copying is necessary. Our use of `const vector<uint8_t>&` parameters helps
to show you where copying needs to happen.

While the collections (`std::vector`, `std::list`, etc.) use the heap, if you
are using them well, you shouldn't have to directly interact with the heap very
much in this assignment.  The reference solution calls `new` only four times,
and two of those times are for managing command-line arguments.  There are five
calls to `delete`, but we provide three of them for you.  Still, you are
expected to ensure your program does not have memory leaks, and to use
`valgrind` to be sure.

As you read through the details of this assignment, keep in mind that subsequent
assignments will build upon this... so as you craft a solution, be sure to think
about how to make it maintainable.  C++ has many features that make it easy to
extend your code.  The `auto` keyword is one.  In-line initialization of arrays
is another.

**Start Early**.  Just reading the code and understanding what is happening
takes time.  If you start reading the assignment early, you'll give yourself
time to think about what is supposed to be happening, and that will help you to
figure out what you will need to do.

When it comes time to implement functions in the client and server, you will
probably want to proceed in the following order: KEY, REG, SAV, BYE, ALL, SET,
GET.  Note that the test script accepts optional `CLIENT`, `SERVER`, and
`VERBOSE` arguments.  The first two will switch to using the solution
executables for the client and server, respectively.  The `VERBOSE` argument
will cause the script to print more information about what it is doing, so that
you can issue the same server and client commands from separate screen sessions
in a single terminal.

As always, please be careful about not committing unnecessary files into your
repository.

Your programs should never require keyboard input. In particular, the client
should get all its parameters from the command line.

Your server should **not** store plain-text passwords in the file.

## Grading

The `scripts` folder has four python scripts that exercise the main portions of
the assignment: basic authentication, file-based persistence, AES encryption,
and RSA encryption.  These scripts use specialized `Makefiles` that integrate
some of the solution code with some of your code, so that you can get partial
credit when certain portions of your code work.  Note that these `Makefiles` are
very strict: they will crash on any compiler warning.  You should *not* turn off
these warnings; they are there to help you.  If your code does not compile with
these scripts, you will not receive any credit.  If you have partial solutions
and you do not know how to coerce the code into being warning-free, you should
ask the professor.

We will also visually inspect your code to ensure you are using salts and
passwords correctly, not accidentally creating TOCTOU vulnerabilities, and so
forth.  While it is our intention to give partial credit, *no credit will be
given for code that does not compile without warnings.*  We also reserve the
right to deduct points for especially poorly written code.  In particular, if
you decide to move functions between files, or add files, or do other things
that break the scripts in the grading folder, then you will lose at least 10%.
If you think some change is absolutely necessary, speak with the professor
first.

Please be sure to use your tools well.  For example, Visual Studio Code (and
emacs, and vim) have features that auto-format your code.  You should use these
features, so that your code is legible.

Broadly speaking, there are five graded components:

* Is the AES cryptography implemented correctly?
* Do the client and server use RSA cryptography correctly?
* Are requests being authenticated properly
* Is persistence implemented according to `format.h`
* Is the implementation of passwords secure and correct?

Remember that the scripts can take VERBOSE, SERVER, and CLIENT arguments.
VERBOSE causes scripts to print output that will be useful when you try to run
the client and server manually.  SERVER and CLIENT cause the scripts to use the
reference solution for the server and client, respectively.  You should use
these to be sure that your implementations are correct.

Note, too, that the graded components are intentionally vague: you need to start
thinking about what it means for code to be "correct".  There are many criteria
that cannot be established through unit testing.

## Notes About the Reference Solution

In the following subsections, you will find some details about the reference
solution.  Note that you should not change any files other than the six that
are listed below.

### `common/my_crypto.cc`

This will probably give more students trouble than any other part of the
assignment, because the OpenSSL documentation is a bit spotty.  The body of
`aes_crypt_message()` is about 20 lines of code.  Knowing when and how to call
`EVP_CipherFinal_ex` is important.  Doing a `CipherUpdate` on a zero-byte block
before calling `CipherFinal` is a bit easier, in terms of the logic, than not.
The code from the tutorial is a good guide, but since that code works on files,
it will need some modification.

### `server/parsing.cc`

In the reference solution, the file is about 120 lines long.  There is one helper
function:

```c++
/// Helper method to check if the provided block of data is a kblock
///
/// @param block The block of data
///
/// @returns true if it is a kblock, false otherwise
bool is_kblock(vec &block);
```

In the reference solution, the `parse_request()` function has about 70 lines of
code.  It employs some nice code-saving techniques, like using an array of
functions when dispatching to the right command:

```c++
  // Iterate through possible commands, pick the right one, run it
  vector<string> s = {REQ_REG, REQ_BYE, REQ_SAV, REQ_SET, REQ_GET, REQ_ALL};
  decltype(handle_reg) *cmds[] = {handle_reg, handle_bye, handle_sav,
                                  handle_set, handle_get, handle_all};
  for (size_t i = 0; i < s.size(); ++i)
    if (cmd == s[i])
      return cmds[i](sd, storage, aes_ctx, ablock);
```

### `client/requests.cc`

In the reference solution, the file is about 310 lines long.  It uses these
two functions to save a small number of lines of repeated code.

```c++
/// Pad a vec with random characters to get it to size sz
///
/// @param v  The vector to pad
/// @param sz The number of bytes to add
///
/// @returns true if the padding was done, false on any error
bool padR(vec &v, size_t sz)
```

```c++
/// Check if the provided result vector is a string representation of ERR_CRYPTO
///
/// @param v The vector being compared to RES_ERR_CRYPTO
///
/// @returns true if the vector contents are RES_ERR_CRYPTO, false otherwise
bool check_err_crypto(const vec &v);
```

In addition, this function is very useful for simplifying `client_get()` and
`client_all()`:

```c++
/// If a buffer consists of OKbbbbd+, where bbbb is a 4-byte binary integer
/// and d+ is a string of characters, write the bytes (d+) to a file
///
/// @param buf      The buffer holding a response
/// @param filename The name of the file to write
void send_result_to_file(const vec &buf, const string &filename);
```

The reference solution also has a handful of functions that handle assembling
the client's request.  You are not required to write helper functions, but you
will probably find that they make it easier to build the client requests. Here's
the declaration for one of the helper files in the reference solution (the
corresponding definition is only 5 lines):

```c++
/// Create unencrypted ablock contents from two strings
///
/// @param s1 The first string
/// @param s2 The second string
///
/// @return A vec representing the two strings
bytevec ablock_ss(const string &s1, const string &s2);
```

The most important helper function is this one.  You'll probably want to
implement something similar:

```c++
/// Send a message to the server, using the common format for secure messages,
/// then take the response from the server, decrypt it, and return it.
///
/// Many of the messages in our server have a common form (@rblock.@ablock):
///   - @rblock padR(enc(pubkey, "CMD".aeskey.length(@msg)))
///   - @ablock enc(aeskey, @msg)
///
/// @param sd  An open socket
/// @param pub The server's public key, for encrypting the aes key
/// @param cmd The command that is being sent
/// @param msg The contents of the @ablock
///
/// @returns a vector with the (decrypted) result, or an empty vector on error
vec send_cmd(int sd, RSA *pub, const string &cmd, const vec &msg);
```

The reference implementation is about 60 lines.  You won't want to write it at
first... figure out how to do REG and SET, and then you'll know what needs to go
in it.  Note that the function will help with REG, BYE, SAV, SET, GET, and ALL.
For example, here's the body of `req_get()` when the helper functions are
done:

```c++
  auto res = send_cmd(sd, pubkey, REQ_GET, ablock_sss(user, pass, getname));
  send_result_to_file(res, getname + ".file.dat");
```

(Yes, just 2 lines... the hard work is in `send_cmd` and
`send_result_to_file`).

### `server/responses.cc`

This file has one function for each of the requests that a client can make.
Each function receives unencrypted data, validates it, sends a request to the
`Storage` object, and then transmits a response to the client.

You may want to write a few helper functions here, too, especially for
extracting the parts of a request.  The reference solution takes about 310 lines
of code to complete this file.  There is a fair bit of copy-and-paste from one
command to the next.

### `server/my_storage.cc`

In the reference solution, this file is about 330 lines (this count includes the
150 lines that are given in the handout).  `persist()` and `load()` take the
most code by far, because file I/O is tedious in C++.  The other methods are
relatively straightforward.  You should not need to add any additional fields to
the `MyStorage` class.

### `server/sequentialmap.h`

In subsequent assignments, you will need to implement your own concurrent hash
map.  In this assignment, we use the `SequentialMap` template as a wrapper
around `std::list`.  This may seem cumbersome, but it means that later on, your
code in `storage.cc` won't have to change: you will just replace the sequential
map with a concurrent hash map that has the same interface.

If you are not familiar with `std::list`, `http://cppreference.com/` is an
excellent resource.  Google will usually send you directly to the correct page.
Note that some of the methods of `SequentialMap` won't be used in this
assignment.  If you figure them out now, then the second assignment will be
easier.  If you skip them, you will not lose points, but you will miss out on
the opportunity to be more prepared for the rest of the semester.

## Collaboration and Getting Help

Students may work in teams of 2 for this assignment.  If you plan to work in a
team, you must notify Prof. Spear by September 6th, so that we can set up your
repository access.  If you do not send a notification after that date, we will
deduct 10 points.  If you are working in a team, you should **pair program** for
the entire assignment.  After all, your goal is to learn together, not to each
learn half the material.

If you require help, you may seek it from any of the following sources:

* The professors and TAs, via office hours or Piazza
* The Internet, as long as you use the Internet as a read-only resource and do
  not post requests for help to online sites.

It is not appropriate to share code with past or current CSE 303 / CSE 403
students, unless you are sharing code with your teammate.

If you are familiar with `man` pages, please note that the easiest way to find a
man page is via Google.  For example, typing `man printf` will probably return
`https://linux.die.net/man/3/printf` as one of the first links.  It is fine to
use Google to find man pages.

StackOverflow is a wonderful tool for professional software engineers.  It is a
horrible place to ask for help as a student.  You should feel free to use
StackOverflow, but only as a *read only* resource.  In this class, you should
**never** need to ask a question on StackOverflow.

## Deadline

You should be done with this assignment before 11:59 PM on September 7th, 2021.
Please be sure to `git commit` and `git push` before that time, so that we can
promptly collect and grade your work.

There are many parts to this assignment, so you will probably want to `git push`
frequently.
