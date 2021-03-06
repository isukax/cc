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
# Unixのプロセス終了コードは0-255までなので負数は返せない
try 0 0
try 42 42
try 41 ' 12 + 34 - 5 '
try 47 "5+6*7"
try 15 "5*(9-6)"
try 4 "(3+5)/2"
try 15 "-3*-5"
try 18 "-(3*-5)+3"
try 1 "3+5==8"
try 0 "3+5!=8"
try 1 "9*2+1<=6+7*5"
try 1 "7/7+5*2>=1-2+5"
try 1 "9*2+1<6+7*5"
try 1 "7/7+5*2>1-2+5"
echo OK