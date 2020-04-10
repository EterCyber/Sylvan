#!/bin/bash

mkdir -p package
docker build -t sylvan-pkg .
docker run -v $PWD/package:/package -t sylvan-pkg

