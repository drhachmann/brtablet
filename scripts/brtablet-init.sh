#!/bin/bash

case "$1" in
  start)
#	 modprobe brtablet-driver
#	 echo "TESTE"
    brtablet-calibrator --start
    ;;
  stop)
#    log_begin_msg "Disabling power management..."
#	  echo "automatico" > /etc/brtablet/arquivo2
#    log_end_msg 0
    ;;
  *)
  ;;
esac

#comando="$(brtablet -v --start $entrada)"



#awk -F : '$1=="path" {print $2}'
