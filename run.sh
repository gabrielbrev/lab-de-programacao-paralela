#!/bin/bash

# $1 = caminho do arquivo .c
# $2 = número de processos (opcional, default = 4)

if [ -z "$1" ]; then
  echo "Uso: $0 arquivo.c [num_processos]"
  exit 1
fi

mkdir -p output

FILE=$1
NP=${2:-4}
OUT="output/$(basename "$FILE" .c)"

mpicc "$FILE" -o "$OUT"
mpirun -np "$NP" "$OUT"