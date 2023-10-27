#!/bin/bash

function run {
  r=$(cnf_lp_deff "$1" | lp_solve -S1 | grep "Value of objective function: " | sed 's/Value of objective function: //g')
  tmp=$(grep -E "^p cnf " "$1" | tail -n 1)
  nv=$(echo "$tmp" | cut -d " " -f 3)
  nc=$(echo "$tmp" | cut -d " " -f 4)
  deff=$(expr "$nc" - "$nv")
  echo "$1, $deff, $r"
}
export -f run

# head -n 450 cnf_list | parallel -n 1 -P 6 run

echo "file, deff, lp"
tail -n 3 cnf_list | parallel -n 1 -P 1 run
