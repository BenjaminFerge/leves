#!/bin/bash -xe
iwyu_tool -p . > iwyu.out
fix_include < iwyu.out
rm iwyu.out
