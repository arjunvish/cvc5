;; HO not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic HO_ALL)
(set-info :status unsat)
(declare-sort $ 0)
(declare-fun t ((-> $ $)) Bool)
(declare-fun p ($) $)
(assert (and (t p) (not (t (lambda ((X $)) (p X))))))
(check-sat)
