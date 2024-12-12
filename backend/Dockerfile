# /********************************************************************************
#  * Copyright (c) 2024 Contributors to the Eclipse Foundation
#  *
#  * See the NOTICE file(s) distributed with this work for additional
#  * information regarding copyright ownership.
#  *
#  * This program and the accompanying materials are made available under the
#  * terms of the Apache License Version 2.0 which is available at
#  * https://www.apache.org/licenses/LICENSE-2.0
#  *
#  * SPDX-License-Identifier: Apache-2.0
#  ********************************************************************************/

# Use the official Ubuntu base image
FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive \
    RUST_VERSION=1.80.0

# Install required packages
RUN apt-get update && apt-get install -y \
    curl \
    build-essential \
    cmake \
    pkg-config \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Rust
RUN curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain ${RUST_VERSION} && \
    echo "source $HOME/.cargo/env" >> $HOME/.bashrc

# Set PATH to include Cargo binaries
ENV PATH="/root/.cargo/bin:${PATH}"

# Set working directory inside the container
WORKDIR /usr/src/app

# Copy Rust code into the container
COPY ./ .

# Build and execute the Rust program
RUN cargo build --release

CMD ["/usr/src/app/target/release/uprotocol_backend"]