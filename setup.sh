#!/bin/bash

author="Antony Kellermann"
username="aokellermann"
name="projectname"
execname="execname"
description="description"

files="README.md PKGBUILD lint.sh LICENSE CMakeLists.txt cmake/__cpptemplate_name__Config.cmake.in
__cpptemplate_name__/test/CMakeLists.txt __cpptemplate_name__/test/unit_test.cc
__cpptemplate_name__/__cpptemplate_name__.cc __cpptemplate_name__/__cpptemplate_name__.h __cpptemplate_name__/main.cc
.circleci/config.yml .github/CONTRIBUTING.md"

sed -i "s/__cpptemplate_name__/$name/g" $files
sed -i "s/__cpptemplate_exec_name__/$execname/g" $files
sed -i "s/__cpptemplate_username__/$username/g" $files
sed -i "s/__cpptemplate_author__/author/g" $files
sed -i "s/__cpptemplate_description__/$description/g" $files

mv "__cpptemplate_name__/__cpptemplate_name__.cc" "__cpptemplate_name__/$name.cc"
mv "__cpptemplate_name__/__cpptemplate_name__.h" "__cpptemplate_name__/$name.h"
mv "cmake/__cpptemplate_name__Config.cmake.in" "cmake/"$name"Config.cmake.in"

mv "__cpptemplate_name__" "$name"
