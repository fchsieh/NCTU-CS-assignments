#!/bin/sh
ls -lRA|grep 'rw'|sort -r -n|awk '{if($1 == "drwxr-xr-x")dirnum += 1;if($1 != "drwxr-xr-x"){printf("%d %s\n", $5, $9);filenum += 1;total += $5;}}END{printf("-1 Dir num: %d\n", dirnum);printf("-1 File num: %d\n", filenum);printf("-1 Total: %d\n", total)}'|sort -rn|awk 'NR > 0 && NR < 6{print NR".", $1, " ", $2}$1 == -1 {print $2, $3, $4}'

