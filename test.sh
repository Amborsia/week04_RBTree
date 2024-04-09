#!/bin/bash

# bstree-test 폴더의 Makefile에서 clean 실행
cd bstree-test
make clean

# bstree-src 폴더의 Makefile에서 clean 실행
cd ../bstree-src
make clean

# bstree-test 폴더로 돌아가서 test 실행
cd ../bstree-test
make test
