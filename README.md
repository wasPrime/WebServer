# WebServer

A C++ Web Server with the technical points below:

- Reactor pattern
- Epoll
- Thread Pool

## Architecture

```mermaid
graph TD

subgraph Server
    subgraph main_reactor
        MainPoller[Poller]
    end

subgraph acceptor
    AcceptSocket
end

acceptor --accept_channel--- main_reactor

subgraph sub_reactor
    subgraph sub_reactor_0
        SubPoller0[Poller]
    end

    subgraph sub_reactor_1
        SubPoller1[Poller]
    end
end

subgraph Connections
    subgraph connection_0
        ConnectionSocket0
    end

    subgraph connection_1
        ConnectionSocket1
    end

    subgraph connection_2
        ConnectionSocket2
    end
end

connection_0 --channel_0--- sub_reactor_0
connection_1 --channel_1--- sub_reactor_1
connection_2 --channel_2--- sub_reactor_1

ThreadPool

sub_reactor_0 --loop--- ThreadPool
sub_reactor_1 --loop--- ThreadPool

end
```

## Build

```bash
mkdir build && cd build
cmake ..
make

make format     # optional
make cpplint    # optional
make clang-tidy # optional
```

## Run

### Echo Service

#### Run echo server

```bash
# at build/
./bin/echo_server
```

#### Run echo client

```bash
# at build/

./bin/echo_client
# or
./bin/echo_clients

```

### Chat service

#### Run chat server

```bash
# at build/
./bin/chat_server
```

#### Run chat client

```bash
# at build/
./bin/chat_client
```
