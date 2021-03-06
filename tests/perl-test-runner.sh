#!/bin/bash

echo '========================================================'

PERL_BINDINGS_DIR=`pwd`/bindings/perl
PERL_PROXIES_DIR=${PERL_BINDINGS_DIR}/PSIToolkit/proxy/
PRJ_DIR=`pwd`/build

if [[ -e $PRJ_DIR/bindings/perl/PSIToolkit/libpsitoolkit_perl_simple.so ]]; then

    echo 'Start testing Perl bindings.'
    echo ''

    export PATH=.:${PRJ_DIR}:${PATH}
# the second path because libSimple.so is looked for in @INC directories
    export PERL5LIB=${PRJ_DIR}/bindings/perl:${PRJ_DIR}/bindings/perl/PSIToolkit:${PERL5LIB}
    export PERL5LIB=${PERL_PROXIES_DIR}:${PERL5LIB}

    echo "PERL5LIB is $PERL5LIB"

    TEST_OUTPUT_DIR=$PRJ_DIR/perl-test-results
    if [[ -d $TEST_OUTPUT_DIR ]]; then
        rm -rf $TEST_OUTPUT_DIR
    fi

    mkdir -p $TEST_OUTPUT_DIR

    export PERL_TEST_HARNESS_DUMP_TAP=$TEST_OUTPUT_DIR

    cd $PRJ_DIR
    perl $PERL_BINDINGS_DIR/Test.PL $PERL_BINDINGS_DIR

else
    echo 'Perl bindings NOT CREATED. Testing perl bindings ignored!'
fi

echo ''
echo '========================================================'
