#!/bin/bash
if [ "${#}" -lt "1" ]
  then
  echo "Usage $0 spoke_number [point_on_spoke]"
  echo "  Spokes are numbered from 1."
  echo "  Points on spokes are numbered from 1."
  echo "  Point 1 is the outermost value, point 7 is the innermost value."
  echo "  (closest to the reference speaker)"
  echo "  Passing 0 as the spoke value gives the reference talker."
  exit -1
fi

# These values are generated using matlab/list_spokes.m which calls
# matlab/get_spoke_points.m

# Central talker is 171.7p112.3s
# Bash-friendly spoke positions generated by list_spokes.m
spokes[0]="171.7p112.3s"
spokes[1]="137.0p104.3s 145.5p106.3s 153.0p108.1s 159.5p109.6s 164.7p110.8s 168.6p111.6s 170.9p112.2s"
spokes[2]="151.6p83.9s 156.7p90.6s 161.1p96.8s 164.9p102.1s 167.8p106.5s 170.0p109.7s 171.3p111.7s"
spokes[3]="180.4p80.1s 178.1p87.6s 176.1p94.5s 174.5p100.6s 173.3p105.6s 172.4p109.3s 171.9p111.5s"
spokes[4]="208.6p93.2s 198.1p97.9s 189.6p102.1s 183.0p105.7s 178.0p108.5s 174.5p110.6s 172.4p111.9s"
spokes[5]="215.2p121.0s 202.7p118.6s 192.7p116.7s 184.8p115.1s 179.0p113.9s 174.9p113.0s 172.5p112.5s"
spokes[6]="194.5p150.4s 188.1p139.2s 183.0p130.4s 178.8p123.6s 175.7p118.5s 173.5p115.0s 172.1p113.0s"
spokes[7]="163.4p157.6s 165.6p144.0s 167.4p133.5s 168.9p125.4s 170.1p119.5s 171.0p115.5s 171.5p113.1s"
spokes[8]="141.3p135.4s 148.8p128.8s 155.5p123.5s 161.1p119.4s 165.7p116.2s 169.0p114.0s 171.0p112.8s"

if [ "${#}" -eq "1" ]
  then
    echo ${spokes[$1]}
    exit 0
  else
    echo `echo ${spokes[$1]} | cut -d " " -f $2`
fi