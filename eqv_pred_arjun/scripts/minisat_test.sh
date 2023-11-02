#!/bin/bash
# grep -E -e "^[0-9]" res.nnf | wc -l

function run {
  #r=$((time -p z3 -dimacs "$1") 2>&1)

  # r=$((time -p minisat "$1") 2>&1)
  # tt=$(echo "$r" | grep "real" | sed 's/real //g')
  # echo "$1, $tt"

  # r=$((wrap 5000 3600 /usr/bin/z3 -dimacs "$1") 2>&1)
  r=$((wrap 5000 3600 /usr/bin/minisat "$1") 2>&1)

  echo "$r" | tail -n 1
}
export -f run

echo "file,state_ms,mem_ms,time_ms"
cat cnf_list | parallel -n 1 -P 1 run

# echo "file, d4_mc, mc, d4_mod_mc, mc_mod"
# head -n 190 cnf_list | parallel -n 1 -P 4 mc_check
# cat list | parallel -n 1 -P 2 run
