#!/bin/bash
set -e

echo "Deploying EcoRecycle System with Docker..."
docker-compose down
docker-compose up --build
