;; Bags are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun A () (Bag Int))
(declare-fun B () (Bag Int))
(declare-fun x () Int)
(assert (= x 1))
(assert (bag.subbag A B))
(assert (bag.subbag B A))
(assert (= (bag.count x A) 5))
(assert (= (bag.count x B) 10))
(check-sat)