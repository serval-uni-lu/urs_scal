#!/bin/bash

function run {
  tmp=$(cnf_to_graph "$1")
  r=$(timeout 600 flow_cutter_pace17 "$1.gr" | grep -E "^s td " | cut -d " " -f 4)

  rm "$1.gr"
  rm "$1.dot"
  rm "$1.red"

  echo "$1, $r"
}
export -f run

echo "file, tw"
head -n 400 cnf_list | parallel -n 1 -P 4 run
