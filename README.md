<h1 align="center">OSTIS Legislation</h1>

[![license](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## About

OSTIS Legislation is an ostis-system designed with [OSTIS Technology](https://github.com/ostis-ai).

The OSTIS Legislation project aims to develop a personal legal assistant, providing individuals with a reliable and accessible source of legal information.

## Docker

### Clone repository

First, clone the repository and navigate to the project directory:

```sh
git clone https://github.com/ostis-apps/ostis-legislation.git
cd ostis-legislation
git checkout develop
git submodule update --init --recursive
```

To use Docker with ostis-legislation, follow these steps:

1. Build the Docker images:

    ```sh
    docker compose build
    ```

2. Build the knowledge base:

    ```sh
    docker compose run --rm machine build
    ```

3. Start the ostis-system: 

    ```sh
    docker compose up
    ```

These commands will set up and run the entire ostis-system using Docker containers. The system will be accessible through the web interface at localhost:8000.

If you need to rebuild the knowledge base after making changes, you can run the second command again. To stop the system, use:

```sh
docker compose down
```

Note that you need to have Docker and Docker Compose installed on your system before running these commands.

## Native

### Clone repository

First, clone the repository and navigate to the project directory:

```sh
git clone https://github.com/ostis-apps/ostis-legislation.git
cd ostis-legislation
git checkout develop
git submodule update --init --recursive
```

### Install pipx

To install pipx, use this guide: https://pipx.pypa.io/stable/installation/.

### Install Conan

Conan is a decentralized package manager for C/C++. It's used in this project to manage dependencies.

```sh
# Use pipx to install conan if not already installed
pipx install conan
pipx ensurepath
```

### Relaunch shell

After installing Conan, relaunch your shell to ensure the new PATH is loaded:

```sh
exec $SHELL
```

### Install sc-machine libraries

sc-machine libraries are the core components of the OSTIS Platform, used to develop C++ agents. They're installed using Conan:

```sh
conan remote add ostis-ai https://conan.ostis.net/artifactory/api/conan/ostis-ai-library
conan profile detect
conan install . --build=missing
```

### Install sc-machine binaries

sc-machine binaries are pre-compiled executables that provide the runtime environment for the ostis-system: build knowledge base source and launch the ostis-system. The installation process differs slightly between Linux and macOS:

#### Linux

```sh
curl -LO https://github.com/ostis-ai/sc-machine/releases/download/0.10.0/sc-machine-0.10.0-Linux.tar.gz
mkdir sc-machine && tar -xvzf sc-machine-0.10.0-Linux.tar.gz -C sc-machine --strip-components 1
rm -rf sc-machine-0.10.0-Linux.tar.gz && rm -rf sc-machine/include
```

#### macOS

```sh
curl -LO https://github.com/ostis-ai/sc-machine/releases/download/0.10.0/sc-machine-0.10.0-Darwin.tar.gz
mkdir sc-machine && tar -xvzf sc-machine-0.10.0-Darwin.tar.gz -C sc-machine --strip-components 1
rm -rf sc-machine-0.10.0-Darwin.tar.gz && rm -rf sc-machine/include
```

### Install Python problem-solver dependencies

To install python dependencies, run:

```sh
python3 -m venv .venv
source .venv/bin/activate
pip3 install -r requirements.txt
```

### Install sc-web

sc-web provides the web-based user interface for the ostis-system. The installation process includes setting up dependencies and building the interface:

#### Ubuntu

```sh
cd interface/sc-web
./scripts/install_deps_ubuntu.sh
npm run build
cd ../..
```

#### macOS

```sh
cd interface/sc-web
./scripts/install_deps_macOS.sh
npm run build
cd ../..
```

## Build

### Build C++ problem solver

The problem solver contains custom agents for your ostis-system. Build it using CMake:

```sh
cmake --preset release-conan
cmake --build --preset release
```

### Build knowledge base

The knowledge base contains your custom knowledge represented in SC-code. It needs to be built before launching the system or after making changes:

```sh
./sc-machine/bin/sc-builder -i repo.path -o kb.bin --clear
```

## Usage

To launch C++ problem solver you should start sc-machine in the first terminal:
```sh
./sc-machine/bin/sc-machine -s kb.bin -e "sc-machine/lib/extensions;build/Release/extensions"
```

To launch Python problem solver you should start sc-machine in the second terminal:
```sh
source .venv/bin/activate && python3 problem-solver/py/server.py
```

After that launch sc-web interface in the third terminal:

```sh
cd interface/sc-web
source .venv/bin/activate && python3 server/app.py
```

To check that everything is fine open localhost:8000 in your browser.
![](https://i.imgur.com/6SehI5s.png)

## Documentation

Run on Linux:
```sh
#Terminal
pip3 install mkdocs markdown-include mkdocs-material
mkdocs serve
```

Then open http://127.0.0.1:8005/ in your browser.

Please note that the documentation for this project is still under development and contains only partial information.

## Project Structure

### Knowledge Base

`knowledge-base` is the place for the knowledge base source text files of your app. Put your .scs and .gwf files here.

After updating your .scs and .gwf files you need to rebuild `knowledge-base`:

```sh
./sc-machine/bin/sc-builder -i repo.path -o kb.bin --clear
```

### Problem Solver

`problem-solver` is the place for the problem solver of your app. Put your agents here. After changes in problem-solver you should rebuild it:

After updating your C++ code you need to rebuild `problem-solver`:

```sh
cmake --preset release-conan
cmake --build --preset release
```

To build C++ code in debug mode, run:

```sh
conan install . --build=missing -s build_type=Debug
cmake --preset debug-conan
cmake --build --preset debug
```

To enable Debug logs set fields in ostis-legislation.ini:

```sh
log_type = Console
log_file = sc-memory.log
log_level = Debug
```

## Codestyle

This project inherits codestyle from sc-machine, which can be found [here](https://ostis-ai.github.io/sc-machine/dev/codestyle/).

## Author

* GitHub: [@ostis-apps](https://github.com/ostis-apps), [@ostis-ai](https://github.com/ostis-ai)

## Show your support

Give us a ⭐️ if you've liked this project!

## 🤝 Contributing

Contributions, issues and feature requests are welcome!<br />Feel free to check [issues page](https://github.com/ostis-apps/ostis-legislation/issues). 

## 📝 License

This project is [MIT](https://opensource.org/license/mit/) licensed.
