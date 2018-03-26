# Chat protocol

This is a simple chat protocol I'm working on every once in a while in ANSI-C just for fun. I try to keep it as pure ANSI-C so that I'm able to compile and run it on my Amiga computer.

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

The message format is inspired by the IRC protocol and the format is as follows:

```
:{access_token}:{nickname}:{message}
```
Newline character determines the end of a message. An example message could be

```
:abcdef1234:hackerman:I'm gonna hack you!
```

## Compiling

Just run `make` and the server and client programs should be compiled.

## Implementation

The current implementation consists of two programs: `server` and `client`. They can be found under the `bin/` directory after a successful compile.

The implementation is extremely simple at this point, in fact, it's not a chat at all yet. It only supports a single client connection and even that is pretty limited in its functionality.
