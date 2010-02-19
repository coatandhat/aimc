#!/bin/bash
find -E . -iregex ".*\.(h|cc|c)" | grep -v "ConvertUTF" | grep -v "SimpleIni" | xargs lint/cpplint.py

