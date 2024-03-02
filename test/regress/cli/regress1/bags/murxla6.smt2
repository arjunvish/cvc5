;; Bags are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-const x (Bag Int))
(assert (distinct (bag.union_disjoint x x) (as bag.empty (Bag Int))))
(assert (= 0 (bag.card x)))
(check-sat)
