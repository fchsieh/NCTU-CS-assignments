#! /bin/sh
# Test zombie process before you shut down your server

OS=$( uname -s )
USER=$( whoami )

[ ${OS} = "FreeBSD" ] && PS_COMMAND="ps -o state"
[ ${OS} = "Linux" ] && PS_COMMAND="ps -u ${USER} -U ${USER} -o state"


if [ -z "$( ${PS_COMMAND} | grep Z )" ]; then
  echo "[0;32mGood Job![m"
else
  echo "[0;31mYou left zombie processes in the system![m"
fi
