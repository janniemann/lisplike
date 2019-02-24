#!/bin/sh
#
# read print loop test
#

_test_stdin() {
    local _num=$1
    local _desc=$2
    local _input=$3
    local _exp_output=$4
    local _exp_status=$5

    echo [TEST] $_desc >&2
    
    OUTPUT=$(echo $_input | scmrpl -)
    STATUS=$?
    if [ X"${STATUS}" != X"${_exp_status}" ] ; then
	echo "not ok $_num - unexpected status [${STATUS}] $_desc [stdin]"
    elif [ X"${OUTPUT}" != X"${_exp_output}" ] ; then
	echo "not ok $_num - unexpected output [${OUTPUT}] $_desc [stdin]"
    else
	echo "ok $_num - $_desc [stdin]"
    fi
}

_test_c() {
    local _num=$1
    local _desc=$2
    local _input=$3
    local _exp_output=$4
    local _exp_status=$5
    
    echo [TEST] $_desc >&2

    OUTPUT=$(scmrpl -c ${_input})
    STATUS=$?
    if [ X"${STATUS}" != X"${_exp_status}" ] ; then
	echo "not ok $_num - unexpected status [${STATUS}] $_desc [buffer]"
    elif [ X"${OUTPUT}" != X"${_exp_output}" ] ; then
	echo "not ok $_num - unexpected output [${OUTPUT}] $_desc [buffer]"
    else
	echo "ok $_num - $_desc [buffer]"
    fi
}


echo "1..34"

_test_stdin 1 number_23 23 23 0
_test_stdin 2 bool_true true "true #t" 0
_test_stdin 3 bool_false false "false #f" 0
_test_stdin 4 string "\"hello world!\"" "\"hello world!\"" 0
_test_stdin 5 prem_eof "\"hello world!" "" 1
_test_stdin 6 sym+p "+p" "+p" 0
_test_stdin 7 sym-p "-p" "-p" 0
_test_stdin 8 sympp "pp" "pp" 0


_test_c 9 number_23 23 23 0
_test_c 10 bool_true true "true #t" 0
_test_c 11 bool_false false "false #f" 0
_test_c 12 string "\"hello world!\"" "\"hello world!\"" 0
_test_c 13 prem_eof "\"hello world!" "" 1
_test_c 14 sym+p "+p" "+p" 0
_test_c 15 sym-p "-p" "-p" 0
_test_c 16 sympp "pp" "pp" 0


_test_stdin 17 "nil nil" "nil" "nil ()" 0
_test_stdin 18 "nil ()" "()" "nil ()" 0
_test_stdin 19 "nil empty list with ws" " (\t\n   )  " "nil ()" 0


_test_stdin 20 "negative integer -236" " \t-236  " "-236" 0
_test_stdin 21 "negative integer -1" " -1\n " "-1" 0
_test_stdin 22 "zero integer -0" " -0\n " "0" 0
_test_stdin 23 "zero integer 0" " \t\n\n0\t " "0" 0
_test_stdin 24 "zero integer +0" " +0\n " "0" 0
_test_stdin 25 "positive integer +1" " +1\n " "1" 0
_test_stdin 26 "positive integer 1" " 1\n " "1" 0
_test_stdin 27 "positive integer +236" " +236\n " "236" 0
_test_stdin 28 "positive integer 236" " 236\n " "236" 0

_test_stdin 29 "neg overflow" "-288230376151711745" "" 1
_test_stdin 30 "neg max" "-288230376151711744" "-288230376151711744" 0
_test_stdin 31 "pos max" "288230376151711743" "288230376151711743" 0
_test_stdin 32 "pos max" "+288230376151711743" "288230376151711743" 0
_test_stdin 33 "pos overflow" "288230376151711744" "" 1
_test_stdin 34 "pos overflow" "+288230376151711744" "" 1

#
# sign whitespace number
#
#
# neg-overflow 
# neg-overflow +1
# pos-overflow -1
# pos-overflow
