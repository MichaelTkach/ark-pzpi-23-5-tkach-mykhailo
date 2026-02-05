FROM ubuntu:latest

# Install dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    libsqlite3-dev \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy project files
COPY . .

# Build the application
RUN mkdir -p build && cd build && \
    cmake .. && \
    make

# Expose port (Server runs on 18081 in Lab 3 code, but Docker maps it)
# NOTE: The user requested 18080 mapping in compose, so we expose the internal port.
# Lab 3 code actually uses 18081 (see main.cpp in Lab 3). 
# We should probably stick to what the code does or map 18081->18080.
# User prompt said: "Server runs on port 18080" in the instructions but Lab 3 used 18081.
# I will expose the port the app listens on (18081) and map relevantly in compose.
EXPOSE 18081

# Run the server
CMD ["./build/EcoRecycleServer"]
