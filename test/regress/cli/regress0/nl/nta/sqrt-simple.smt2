;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_NRAT)
(set-info :status unsat)
(declare-fun x () Real)
(assert (> x 0.0))
(assert (not (= (* (sqrt x) (sqrt x)) x)))
(check-sat)
