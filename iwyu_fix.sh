#!/bin/bash -xe
iwyu_tool -j8 -p . -- -Xiwyu --no_fwd_decls > iwyu.out
fix_include < iwyu.out
rm iwyu.out
