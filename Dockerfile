FROM ubuntu:noble AS base

ENV CCACHE_DIR=/ccache
USER root

COPY scripts /app/scripts
COPY conanfile.py /app/conanfile.py
COPY CMakePresets.json /app/CMakePresets.json
COPY CMakeLists.txt /app/CMakeLists.txt
COPY requirements.txt /app/requirements.txt

# tini is an init system to forward interrupt signals properly
RUN apt update && apt install -y --no-install-recommends sudo tini curl ccache python3 python3-pip pipx cmake build-essential ninja-build

# Install Conan
RUN pipx install conan && \
    pipx ensurepath

FROM base AS devdeps
WORKDIR /app

SHELL ["/bin/bash", "-c"]
RUN python3 -m venv /app/.venv && \
    source /app/.venv/bin/activate && \
    pip3 install -r /app/requirements.txt

ENV PATH="/root/.local/bin:$PATH"
RUN conan remote add ostis-ai https://conan.ostis.net/artifactory/api/conan/ostis-ai-sc-machine && \
    conan profile detect && \
    conan install . --build=missing

# Install sc-machine binaries
RUN curl -LO https://github.com/ostis-ai/sc-machine/releases/download/0.10.0/sc-machine-0.10.0-Linux.tar.gz && \
    mkdir sc-machine && tar -xvzf sc-machine-0.10.0-Linux.tar.gz -C sc-machine --strip-components 1 && \
    rm -rf sc-machine-0.10.0-Linux.tar.gz && rm -rf sc-machine/include

FROM devdeps AS devcontainer
RUN apt install -y --no-install-recommends cppcheck valgrind gdb bash-completion ninja-build curl
ENTRYPOINT ["/bin/bash"]

FROM devdeps AS builder
COPY . .
RUN --mount=type=cache,target=/ccache/ cmake --preset release-conan && cmake --build --preset release

# Gathering all artifacts together
FROM base AS final

COPY --from=builder /app/scripts /app/scripts
COPY --from=builder /app/sc-machine /app/sc-machine
COPY --from=builder /app/build/Release/extensions /app/build/Release/extensions
COPY --from=builder /app/.venv /app/.venv

WORKDIR /app

EXPOSE 8090

ENTRYPOINT ["/usr/bin/tini", "--", "/app/scripts/docker_entrypoint.sh"]
