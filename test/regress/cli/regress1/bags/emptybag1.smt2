;; Bags are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun A () (Bag String))
(declare-fun x () String)
(declare-fun y () Int)
(assert (= x "x"))
(assert (= A (as bag.empty (Bag String))))
(assert (= (bag.count x A) y))
(assert(> y 1))
(check-sat)
