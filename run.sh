#!/bin/bash

# $1 = caminho do arquivo .c
# $2 = número de processos (opcional, default = 4)
# $3... = argumentos extras para o programa

if [ -z "$1" ]; then
  echo "Uso: $0 arquivo.c [num_processos] [args...]"
  exit 1
fi

mkdir -p output

FILE=$1
NP=${2:-4}

# Pega os argumentos extras (a partir do 3º argumento)
shift 2
ARGS="$@"

OUT="output/$(basename "$FILE" .c)"

mpicc "$FILE" -o "$OUT"
mpirun -np "$NP" "$OUT" $ARGS