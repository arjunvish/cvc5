;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(set-option :produce-models true)
(declare-fun A () (Set Int))
(assert (distinct (set.choose A) (set.choose A)))
(check-sat)