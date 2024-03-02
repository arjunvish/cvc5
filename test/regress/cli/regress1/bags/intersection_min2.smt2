;; Bags are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun A () (Bag String))
(declare-fun B () (Bag String))
(declare-fun C () (Bag String))
(assert (= C (bag.inter_min A B)))
(assert (= C (bag.union_disjoint A B)))
(assert (distinct (as bag.empty (Bag String)) C))
(check-sat)
