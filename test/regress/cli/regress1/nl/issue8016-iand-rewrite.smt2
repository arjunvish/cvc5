;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun v () Int)
(assert (exists ((V Int)) (and (= 2 v) (= v ((_ iand 1) 1 v)))))
(check-sat)
