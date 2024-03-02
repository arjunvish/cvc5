;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun S () (Set Int))
(declare-fun T () (Set Int))
(declare-fun x () Int)
(declare-fun y () Int)
(assert (set.member y S))
(assert (not (set.member x (set.union S T))))
(assert (= x y))
(check-sat)
