default:
    just --list

docker-build-backend:
    docker build \
        --no-cache \
        -t wiseriders \
        --file=./backend/Dockerfile \
        ./backend

docker-run-backend-shell:
    docker run \
        --rm -ti \
        -v ./:/opt/repo \
        --name wiseriders-shell \
        wiseriders \
        bash

docker-run-backend:
    docker run \
        --rm -ti \
        --name wiseriders \
        wiseriders
