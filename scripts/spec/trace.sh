#!/usr/bin/env bash

DIR=$(realpath "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && echo $(pwd)/../../)")
mkdir -p $DIR/tmp/trace
cmds=$DIR/tmp/trace/trace.sh
rm -f $(dirname $cmds)/*

for i in `find $DIR/spec/benchspec -maxdepth 3 -type d | grep run`; do
  benchmark=$(basename $(dirname $i))
  speccmds=$(find $i | grep -E "speccmds\.cmd" | head -n 1)
  echo $benchmark

  # Copy executable
  rundir=$(dirname $speccmds)
  cp $rundir/../../exe/* $rundir

  # Convert commands
  invoke=$(dirname $cmds)/$benchmark.sh
  $DIR/spec/bin/specinvoke -n $speccmds >> $invoke

  # Delete last line (speccmds exit: rc=1)
  sed -i '$d' $invoke

  # Remove all comments
  sed -i '/^#/d' $invoke

  # Export OMP variables every time
  sed -i "s@^@pin -a '-f $benchmark.trace.gz -m $benchmark.meta -p $benchmark.progress -i $DIM' -d $2 -e @" $invoke

  # Only take the first output
  echo $(head $invoke -n 1) >> $cmds
done

