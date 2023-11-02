#!/bin/bash
# grep -E -e "^[0-9]" res.nnf | wc -l

function run {
  r=$(grep -E "^p cnf " "$1" | head -n 1)

  nbv=$(echo "$r" | cut -d ' ' -f 3)
  nbc=$(echo "$r" | cut -d ' ' -f 4)

  nbl=$(grep -v -E "^[cp]" "$1" | sed 's/ 0$//g' | grep -o -E "[-]*[0-9]*" | wc -l)

  echo "$1, $nbv, $nbc, $nbl"
}
export -f run

echo "file,#var,#clause,#literal"
cat cnf_list | parallel -n 1 -P 6 run
