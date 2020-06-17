#!/bin/bash

PROJECT_PATH=$1
PORT1=$2
PORT2=$3
PORT3=$4

if [ -z "$PROJECT_PATH" ] || [ -z "$PORT1" ] || [ -z "$PORT2" ] || [ -z "$PORT3" ]; then
  echo "Usage: $0 <project_path> <port1> <port2> <port3>"
  exit 1
fi

tmux set remain-on-exit on

make -C $PROJECT_PATH

tmux split-window -v -p 95
tmux split-window -v -p 70
tmux split-window -v -p 50

tmux select-pane -t 1
./demo_task.sh $PROJECT_PATH/np_simple $PORT1 1 2
tmux select-pane -t 3
./demo_task.sh $PROJECT_PATH/np_single_proc $PORT2 3 4
tmux select-pane -t 5
./demo_task.sh $PROJECT_PATH/np_multi_proc $PORT3 5 6

tmux kill-pane -t 0
