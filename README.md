<h1 align="center">OSTIS Legislation</h1>

[![license](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## About

OSTIS Legislation is an ostis-system designed with [OSTIS Technology](https://github.com/ostis-ai).

The OSTIS Legislation project aims to develop a personal legal assistant, providing individuals with a reliable and accessible source of legal information.

### Prerequisites

Ensure these tools are installed before proceeding:

#### General Prerequisites

Required for both Docker and Native installations:

*   **Git:**  For cloning the repository.
    [https://git-scm.com/book/en/v2/Getting-Started-Installing-Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)

## Docker Setup (Recommended)

Docker simplifies setup and provides a consistent environment.

1.  **Install Docker and Docker Compose:**

    Instructions: [https://www.docker.com/get-started/](https://www.docker.com/get-started/).

2.  **Clone repository:**

    ```sh
    git clone https://github.com/ostis-apps/ostis-legislation.git
    cd ostis-legislation
    git checkout develop
    git submodule update --init --recursive
    ```

3.  **Build Docker images:**

    ```sh
    docker compose build
    ```

    This command creates the necessary Docker images for the application.

4.  **Build knowledge base:**

    ```sh
    docker compose run --rm machine build
    ```

    The `--rm` flag ensures the container is removed after the build.

5.  **Start ostis-system:**

    ```sh
    docker compose up
    ```
    
    This command starts all the services defined in the `docker-compose.yml` file (sc-machine, sc-web). The system is accessible at `localhost:8000`.

6.  **Stop ostis-system:**

    ```sh
    docker compose stop
    ```

    This command stops and removes the containers created by `docker compose up`. To fully remove the containers, you can use `docker compose down` instead.

    *Important: Rebuild the knowledge base (step 4) after any changes to the knowledge base files.*

## Native Installation

Steps for installing and running the application directly on your system.

1.  **Install basic tools for development environment:**

    *   **Ubuntu/Debian (GCC):** 
        
        ```sh
        sudo apt update
        
        sudo apt install --yes --no-install-recommends \
            curl \
            ccache \
            python3 \
            python3-pip \
            build-essential \
            ninja-build
        ```
        
    *   **macOS (Clang):**

        ```sh
        brew update && brew upgrade
        brew install \
            curl \
            ccache \
            cmake \
            ninja
        ```

    *   **Other Linux distributions:**

        If you're using a different Linux distribution that doesn't support apt, ensure you have equivalent packages installed:

        * curl: A tool for transferring data with URLs;
        * ccache: A compiler cache to speed up compilation processes;
        * python3 and python3-pip: Python 3 interpreter and package installer;
        * build-essential: Includes a C++ compiler, necessary for building C++ components;
        * ninja-build: An alternative build system designed to be faster than traditional ones.

    Compiler is required for building C++ components.

2.  **Install pipx:**

    Instructions: [https://pipx.pypa.io/stable/installation/](https://pipx.pypa.io/stable/installation/).
    
    `pipx` isolates Python packages, preventing conflicts, especially useful when working with tools like CMake and Conan.

3.  **Install CMake:**

    ```sh
    pipx install cmake
    pipx ensurepath
    ```
   
    CMake is used to generate build files for your specific system. `pipx ensurepath` adds CMake to your PATH.

4.  **Install Conan:**

    ```sh
    pipx install conan
    pipx ensurepath
    ```
    
    Conan manages the project's C++ dependencies. `pipx ensurepath` adds Conan to your PATH.

5.  **Clone repository:**

    ```sh
    git clone https://github.com/ostis-apps/ostis-legislation.git
    cd ostis-legislation
    git checkout develop
    git submodule update --init --recursive
    ```

6.  **Restart your shell:**

    ```sh
    exec $SHELL
    ```
    
    Ensures that the PATH changes from `pipx ensurepath` are applied.

7.  **Install C++ problem solver dependencies:**

    They include sc-machine libraries -- the core components of the OSTIS Platform, used to develop C++ agents. They're installed using Conan:

    ```sh
    conan remote add ostis-ai https://conan.ostis.net/artifactory/api/conan/ostis-ai-library
    conan profile detect
    conan install . --build=missing
    ```
    
    `--build=missing` builds dependencies from source if pre-built binaries are not available.

8.  **Install sc-machine binaries:**
   
    sc-machine binaries are pre-compiled executables that provide the runtime environment for the ostis-system: build knowledge base source and launch the ostis-system. The installation process differs slightly between Linux and macOS:

    *   **Linux:**

        ```sh
        curl -LO https://github.com/ostis-ai/sc-machine/releases/download/0.10.0/sc-machine-0.10.0-Linux.tar.gz
        mkdir sc-machine && tar -xvzf sc-machine-0.10.0-Linux.tar.gz -C sc-machine --strip-components 1
        rm -rf sc-machine-0.10.0-Linux.tar.gz && rm -rf sc-machine/include
        ```

    *   **macOS:**

        ```sh
        curl -LO https://github.com/ostis-ai/sc-machine/releases/download/0.10.0/sc-machine-0.10.0-Darwin.tar.gz
        mkdir sc-machine && tar -xvzf sc-machine-0.10.0-Darwin.tar.gz -C sc-machine --strip-components 1
        rm -rf sc-machine-0.10.0-Darwin.tar.gz && rm -rf sc-machine/include
        ```
    
    Downloads and extracts pre-built `sc-machine` binaries for your operating system. The `include` directory is removed because it is not required.

9.  **Install Python problem-solver dependencies:**

    To install python dependencies, run:

    ```sh
    python3 -m venv .venv
    source .venv/bin/activate
    pip3 install -r requirements.txt
    ```

10. **Install sc-web:**

    sc-web provides the web-based user interface for the ostis-system. The installation process includes setting up dependencies and building the interface:

    *   **Ubuntu/Debian:**

        ```sh
        cd interface/sc-web
        ./scripts/install_deps_ubuntu.sh
        npm install  # Ensure npm dependencies are installed
        npm run build
        cd ../..
        ```

    *   **macOS:**

        ```sh
        cd interface/sc-web
        ./scripts/install_deps_macOS.sh
        npm install  # Ensure npm dependencies are installed
        npm run build
        cd ../..
        ```
    
    Installs the necessary dependencies for the web interface. `npm install` downloads JavaScript packages, and `npm run build` compiles the web interface.

## Building ostis-system

1.  **Build problem solver:**
   
    The problem solver contains custom agents for your ostis-system. Build it using CMake:

    ```sh
    cmake --preset release-conan
    cmake --build --preset release
    ```
    
    These commands use CMake to build the C++ problem solver in Release mode. The `--preset` option specifies a pre-configured build setup.

2.  **Build knowledge base:**

    The knowledge base contains your custom knowledge represented in SC-code. It needs to be built before launching the system or after making changes:

    ```sh
    ./sc-machine/bin/sc-builder -i repo.path -o kb.bin --clear
    ```
    
    This command builds the knowledge base from the `.scs` and `.gwf` files in the `knowledge-base` directory, creating the `kb.bin` file. The `--clear` flag clears the knowledge base before building.

## Running ostis-system

1.  **Start `sc-machine` (in a terminal):**

    ```sh
    ./sc-machine/bin/sc-machine -s kb.bin -e "sc-machine/lib/extensions;build/Release/extensions"
    ```
    
    Starts the `sc-machine`, loading the knowledge base (`kb.bin`) and specifying the paths to the extensions.

2.  **Start `sc-web` interface (in a separate terminal):**

    ```sh
    cd interface/sc-web
    source .venv/bin/activate && python3 server/app.py
    ```
    
    Starts the `sc-web`. `source .venv/bin/activate` activates the virtual environment for `sc-web`, and `python3 server/app.py` starts the web server.

3.  **Start Python problem solver (in a separate terminal):**

    ```sh
    source .venv/bin/activate && python3 problem-solver/py/server.py
    ```

4.  **Access interface:** Open `localhost:8000` in your web browser.

    ![Example Screenshot](https://i.imgur.com/6SehI5s.png)

To stop the running servers for the ostis-legislation, press `Ctrl+C` in the terminals where sc-machine and sc-web are running.

## Project Structure

*   **`knowledge-base`**: Contains the knowledge base source files (`.scs`, `.gwf`). Rebuild the knowledge base after making changes:

    ```sh
    ./sc-machine/bin/sc-builder -i repo.path -o kb.bin --clear
    ```

*   **`problem-solver`**: Contains the C++ agents that implement the problem-solving logic. Rebuild after modifying:

    ```sh
    cmake --preset release-conan
    cmake --build --preset release
    ```

    For debug mode:

    ```sh
    conan install . --build=missing -s build_type=Debug
    cmake --preset debug-conan
    cmake --build --preset debug
    ```

    For release mode with tests:

    ```sh
    cmake --preset release-with-tests-conan
    cmake --build --preset release
    ```

    To enable debug logs, configure `ostis-legislation.ini`:

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
