#!/usr/bin/env bash

DIR=$(realpath "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && echo $(pwd)/../../)")
cmds=$DIR/tmp/simpoint.sh

rm -f $cmds

for i in `find $DIR/tmp -type f | grep bbv`; do
  echo $i

  benchmark=`basename $i | cut -d "." -f 1,2`
  dir=$(dirname $i)/$benchmark
  mkdir -p $dir

  echo "simpoint -b $i -m 50 -o $dir" >> $cmds
done
