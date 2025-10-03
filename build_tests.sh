#!/bin/bash

mkdir -p bin

echo "🔨 Compilando programas de prueba..."

gcc -o bin/test_crash test/test_crash.c
gcc -o bin/test_normal test/test_normal.c
gcc -o bin/test_sleep test/test_sleep.c
gcc -o bin/test_spam test/test_spam.c

chmod +x bin/*

echo "✅ Programas de prueba compilados en bin/"