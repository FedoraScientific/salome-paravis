#!/bin/bash

CONF_DIR=`echo $0 | sed -e "s,[^/]*$,,;s,/$,,;s,^$,.,"`

cp $1/hints hints
cat ${CONF_DIR}/hints_paravis >> hints
