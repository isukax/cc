#!/bin/bash
try() 
{
    expected="$1"
    input="$2"

    # コンパイル
    ./main "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

# テスト
try 0 0
try 42 42
try 41 ' 12 + 34 - 5 '

echo OK