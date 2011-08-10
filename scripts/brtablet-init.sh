#!/bin/bash

case "$1" in
  start)
    sudo brtablet --start
	 echo "AA" > /etc/brtablet/AAA
    ;;
  stop)
	sudo brtablet --stop;;
  *)
  ;;
esac

#comando="$(brtablet -v --start $entrada)"



#awk -F : '$1=="path" {print $2}'
