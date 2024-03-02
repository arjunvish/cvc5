;; Bags are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun A () (Bag String))
(declare-fun B () (Bag String))
(assert (= B (bag.setof A)))
(assert (distinct (as bag.empty (Bag String)) A B))
(assert (= B (bag.union_max A B)))
(check-sat)