# TTT - terminal typing test
## Goal:
Perform a an offline typing test with useful data analays.

## Features:
- accurate typing test of 15, 30, 60, 120 seconds, for 200,1000 most common english words.
- offline
- stats: best wpm/accuracy in categories 


## Notes:

* it only works on linux
* The code is super messy. I made it super bloated than it should've been.

## Instalation:

There are two ways:
1. Docker-way:
Build and run the image without installing any other dependencies on your machine
```
$ git clone https://github.com/aizyuval/termtypetest.git
$ cd termtypetest
$ docker build -t typetest .
$ docker run typetest:latest
```
2. Source way:
Build and run the code on your machine.

### Must Have Dependencies:
[ncurses](https://invisible-island.net/ncurses/#downloads)

[libyaml](https://github.com/yaml/libyaml)

[libcyaml](https://github.com/tlsa/libcyaml?tab=readme-ov-file)

### run:
```
$ cd ~
$ git clone github/aizyuval/termtypetest.git
$ cd termtypetest
$ sudo make
$ typetest
```

