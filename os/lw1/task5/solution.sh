#!/usr/bin/env bash
set -e

if test -d out;
then rm -rf out/*
else mkdir out
fi

cd out

whoami > me.txt

cp me.txt metoo.txt

man wc > wchelp.txt

cat wchelp.txt

wc -l wchelp.txt | cut -d ' ' -f 1 > wchelp-lines.txt

tac wchelp.txt > wchelp-reversed.txt

cat wchelp.txt wchelp-reversed.txt me.txt metoo.txt wchelp-lines.txt > all.txt

tar -cf result.tar *.txt
gzip -k result.tar

cd ..
if test -f result.tar.gz;
  then rm result.tar.gz
fi
mv out/result.tar.gz .
rm -r out