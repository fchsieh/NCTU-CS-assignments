#!/bin/bash

SERVER_PATH=$1
SERVER_PORT=$2
CLIENT_PANE=$3
SERVER_PANE=$4

if [ -z "$SERVER_PATH" ] || [ -z "$SERVER_PORT" ]; then
  echo "Usage: $0 <server_path> <port>"
  exit 1
fi

SERVER_PATH=$( readlink -f ${SERVER_PATH} )

case $SERVER_PATH in
*np_simple)
  TESTCASE_PATH="${PWD}/client/single_client"
  ;;
*np_single_proc)
  TESTCASE_PATH="${PWD}/client/multi_client"
  ;;
*np_multi_proc)
  TESTCASE_PATH="${PWD}/client/multi_client_2"
  ;;
*)
  echo 'Wrong server name.'
  exit 1
  ;;
esac

if [ ! -x "$SERVER_PATH" ] ; then
  echo "Your ${SERVER_PATH} server is not executable."
  exit 1
fi

if [ -z "$CLIENT_PANE" ] ; then
  tmux new-session -d -s "np_demo_task"
  CLIENT_PANE=1
  SERVER_PANE=2
  tmux split-window -v
fi

tmux split-window -h

tmux send-keys -t $CLIENT_PANE "clear" Enter
tmux send-keys -t $SERVER_PANE "clear" Enter

tmux send-keys -t $SERVER_PANE "cd $PWD/server" Enter
tmux send-keys -t $SERVER_PANE "make clean; ./server.sh $SERVER_PATH $SERVER_PORT" Enter

sleep 1.5
tmux send-keys -t $CLIENT_PANE "cd $TESTCASE_PATH" Enter
tmux send-keys -t $CLIENT_PANE "./demo.sh 127.0.0.1 $SERVER_PORT" Enter
