FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    libsqlite3-dev \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN mkdir -p build && cd build && \
    cmake .. && \
    make

EXPOSE 18081

CMD ["./build/EcoRecycleServer"]
