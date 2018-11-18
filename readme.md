# Chat protocol

This is a simple chat protocol I'm working on every once in a while in ANSI-C just for fun. I try to keep it as pure ANSI-C so that I'm able to compile and run it on my Amiga computer.

## Requirements

### Mac and Linux

[Curses](https://en.wikipedia.org/wiki/Curses_(programming_library))

### Amiga

SAS/C 6.50+

[curses 2.10](http://aminet.net/package/dev/misc/curses210)

[AmiTCP SDK](http://aminet.net/package/comm/tcp/AmiTCP-SDK-4.3)

You may need to change the library locations in the smakefile to fit your installation.

## Protocol description

### Handshake

The handshake is as follows.

After connecting to the server the client sends:

```
AHOY
```

The server responds with:

```
AHOY-HOY:{access_token}
```
Where the access token is a random string, for example


```
AHOY-HOY:abcdef1234
```

The handshake is now complete and the client should store the access_token for use with subsequent messages.

### Message format

The message format is vaguely inspired by the IRC protocol and the format is as follows:

```
:{access_token}:{nickname}:{message_type}:{message}
```

Newline character determines the end of a message. Regular chat messages have the message_type "normal". An example message could be

```
:abcdef1234:hackerman:normal:I'm gonna hack you!
```

### Command messages

Commands have the message_type "command". Chat history could be requested with:

```
:abcdef1234:hackerman:command:history
```

## Compiling

### Mac and Linux

Run `make` and the server and client programs should be compiled.

### Amiga

Run `smake` and the client should be compiled. Server is not yet ported for the Amiga.

## Implementation

The current implementation consists of two programs: `server` and `client`. They can be found under the `bin/` directory after a successful compile.

The server runs on the port 8002. You can provide an IP address as a command line argument for the client. If no argument is provided "127.0.0.1" is used by default. Only IP addresses are accepted at the moment (no domains).

Multiple clients can connect to the server and they see each others messages.

