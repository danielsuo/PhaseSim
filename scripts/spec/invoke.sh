#!/usr/bin/env bash

DIR=$(realpath "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && echo $(pwd)/../../)")
cmds=$DIR/tmp/spec/cmds.sh
rm -f $(dirname $cmds)/*

for i in `find $DIR/spec/benchspec -maxdepth 3 -type d | grep run`; do
  benchmark=$(basename $(dirname $i))
  speccmds=$(find $i | grep speccmds)

  if grep -Fxq $benchmark ~/PhaseSim/process.txt
  then
    echo Processing $benchmark
  else
    continue
  fi

  # Copy executable
  rundir=$(dirname $speccmds)
  cp $rundir/../../exe/* $rundir

  # Convert commands
  echo $speccmds
  invoke=$(dirname $cmds)/$benchmark.sh
  $DIR/spec/bin/specinvoke -n $speccmds >> $invoke

  # Delete last line (speccmds exit: rc=1)
  sed -i '$d' $invoke

  # Remove all comments
  sed -i '/^#/d' $invoke

  #j=1
  #while read line; do
    #echo $line $j
    #((j++))
  #done < $invoke
  
  # Form commands
  bbv=$(dirname $cmds)/$benchmark.bbv
  touch $bbv

  # Export OMP variables every time
  sed -i "s@^@export OMP_NUM_THREADS=1 \&\& cd $rundir \&\& valgrind --tool=exp-bbv --bb-out-file=$bbv @" $invoke

  # Only take the first output
  echo $(head $invoke -n 1) | tee -a $cmds
done

