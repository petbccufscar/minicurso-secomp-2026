FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    python3-pip \
    python3-dev \
    build-essential \
    libssl-dev \
    libffi-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# Instala o radare2 a partir do repositório oficial
RUN git clone https://github.com/radareorg/radare2.git \
    && cd radare2 \
    && sys/install.sh \
    && cd .. \
    && rm -rf radare2

RUN pip3 install --no-cache-dir pwntools

RUN git clone https://github.com/pwndbg/pwndbg /opt/pwndbg \
    && cd /opt/pwndbg && ./setup.sh

WORKDIR /workspace
CMD ["tail", "-f", "/dev/null"]