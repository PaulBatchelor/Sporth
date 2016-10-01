#!/bin/bash
PS_PATH=/usr/local/share/sporth/polysporth
mkdir -p $PS_PATH
install ugens/polysporth/scm/init.scm $PS_PATH
install ugens/polysporth/scm/ps.scm $PS_PATH
