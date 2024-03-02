;; Bags are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun A () (Bag Int))
(declare-fun B () (Bag Int))
(declare-fun x () Int)
(declare-fun y () Int)
(assert (= A (bag.union_max (bag x 1) (bag y 2))))
(assert (= A (bag.union_disjoint B (bag y 2))))
(assert (= x y))
(assert (distinct (as bag.empty (Bag Int)) B))
(check-sat)