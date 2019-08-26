# LinuxMemory

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/0f2d892835f44b6c9a599b39e8155eeb)](https://app.codacy.com/app/PierreLeGuen/LinuxMemory?utm_source=github.com&utm_medium=referral&utm_content=PierreLeGuen/LinuxMemory&utm_campaign=Badge_Grade_Dashboard)
[![CodeFactor](https://www.codefactor.io/repository/github/pierreleguen/linuxmemory/badge)](https://www.codefactor.io/repository/github/pierreleguen/linuxmemory)

Linux game memory editing API, written in C. This project is mainly for _memory editing_ of **singleplayer** video games, but it can be used on any software under Unix.

## Goal

The objective of this project is to open the world of external memory editing utilities under Linux.

## Getting started

### With binaries

- Download the binaries from here: <https://github.com/PierreLeGuen/LinuxMemory/releases>
- Unzip the release in your project
- Finally include `"LinuxMemory.h"`

### OR Build sources

Make sure you have `cmake` (and make) prior to build.

- Clone the repo
- Run  `cmake .`
- Build with `make`
- You should see the folder `LinuxMemoryAPI` appears at the root of the project.
-   Copy paste this folder to your project and include `"LinuxMemory.h"`

## How to use the API

See inside the examples. For now, only one simple external _hack_ can be found, for the game [assault cube](https://assault.cubers.net/).
