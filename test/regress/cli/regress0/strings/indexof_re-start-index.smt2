; COMMAND-LINE: --strings-exp
;; operator str.indexof_re_a not supported
; DISABLE-TESTER: alethe
(set-logic QF_SLIA)
(declare-fun i () Int)
(declare-fun a () String)
(assert (= i (str.indexof_re a (str.to_re "abc") 3)))
(assert (and (>= i 0) (< i 3)))
(set-info :status unsat)
(check-sat)
