#!/usr/bin/bash

# rbtree-test 폴더의 Makefile에서 clean 실행
cd test
make clean

# rbtree-src 폴더의 Makefile에서 clean 실행
cd ../src
make clean

# bstree-test 폴더로 돌아가서 test 실행
cd ../test
make test
