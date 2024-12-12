default:
    just --list

docker-build-backend:
    docker build \
        --no-cache \
        -t wiseriders/backend \
        --file=./backend/Dockerfile \
        ./backend

docker-run-backend-shell:
    docker run \
        --rm -ti \
        -v ./:/opt/repo \
        --name wiseriders_backend-shell \
        wiseriders/backend \
        bash

docker-run-backend:
    docker run \
        --rm -ti \
        --name wiseriders_backend \
        wiseriders/backend
