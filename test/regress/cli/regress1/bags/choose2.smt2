;; Bags are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun A () (Bag Int))
(assert (distinct (bag.choose A) (bag.choose A)))
(check-sat)
