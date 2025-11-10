#!/bin/bash

# $1 = caminho do arquivo .c
# $2... = argumentos extras para o programa

if [ -z "$1" ]; then
  echo "Uso: $0 arquivo.c [args...]"
  exit 1
fi

mkdir -p output

FILE=$1
shift
ARGS="$@"

OUT="output/$(basename "$FILE" .c)"

# Compila com GCC 15 e OpenMP
/opt/homebrew/bin/gcc-15 -fopenmp "$FILE" -o "$OUT"

# Roda o programa
"$OUT" $ARGS