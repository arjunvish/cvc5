;; Bags are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic HO_ALL)
(set-info :status unsat)
(declare-fun A () (Bag Int))
(declare-fun B () (Bag Int))
(declare-fun element () Int)
(declare-fun p (Int) Bool)
(assert (= B (bag.filter p A)))
(assert (p element))
(assert (not (bag.member element A)))
(assert (bag.member element B))
(check-sat)
