# ── Build stage ──────────────────────────────────────────────────────────────
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
        gcc \
        cmake \
        make \
        libgl1-mesa-dev \
        libglu1-mesa-dev \
        freeglut3-dev \
        libxmu-dev \
        libxi-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .

RUN cmake -B build -S . && cmake --build build

# ── Runtime stage ─────────────────────────────────────────────────────────────
FROM ubuntu:22.04 AS runtime

RUN apt-get update && apt-get install -y --no-install-recommends \
        libgl1-mesa-glx \
        libglu1-mesa \
        freeglut3 \
        libxmu6 \
        libxi6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /build/build/senior-playground .
COPY --from=builder /build/assets ./

# The application renders to an X11 display.
# Pass DISPLAY at runtime and mount the X11 socket, e.g.:
#   docker run --rm -e DISPLAY=$DISPLAY \
#              -v /tmp/.X11-unix:/tmp/.X11-unix \
#              senior-playground
CMD ["./senior-playground"]
