; COMMAND-LINE: --strings-exp
;; introduces RE Skolem
; DISABLE-TESTER: alethe
(set-logic QF_SLIA)
(set-info :status unsat)
(declare-const x String)
(assert (str.in_re x (re.++ (str.to_re "abc") re.all)))
(assert (not (str.prefixof "abc" x)))
(check-sat)
