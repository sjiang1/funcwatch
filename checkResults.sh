#!/bin/bash

echo "Remove empty files."

find . -size 0 -print0 |xargs -0 rm
