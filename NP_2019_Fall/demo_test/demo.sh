#!/bin/bash

PROJECT_PATH=$1
PORT1=$2
PORT2=$3
PORT3=$4

if [ -z "$PROJECT_PATH" ] || [ -z "$PORT1" ] || [ -d "$PROJECT_PATH" ] && [ -z "$PORT3" ] ; then
  printf "Usage: $0 <project_path> <port1> <port2> <port3>\n       $0 <server_path> <port>\n"
  exit 1
fi

if [ -z "$TMUX" ]; then
  if [ -n "`tmux ls | grep np_demo`" ]; then
    tmux kill-session -t np_demo
  fi
  if [ -d "$1" ]; then
    tmux new-session -s np_demo -n np_demo_sample "cd $PWD; ./demo_tmux.sh $1 $2 $3 $4"
  else
    tmux new-session -s np_demo -n np_demo_sample \
    "tmux split-window -v -p 95; tmux split-window -v -p 55; cd $PWD; ./demo_task.sh $1 $2 2 3"
  fi
else
  tmux new-window -n np_demo_sample
  if [ -d "$1" ]; then
    ./demo_tmux.sh $1 $2 $3 $4
  else
    ./demo_task.sh $1 $2
  fi
fi
