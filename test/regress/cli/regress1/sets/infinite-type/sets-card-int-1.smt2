;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_ALL)
(set-option :sets-ext true)
(set-info :status unsat)
(assert (= (set.card (as set.universe (Set Int))) 10))
(declare-const universe (Set Int))
(declare-const A (Set Int))
(declare-const B (Set Int))
(assert (= (set.card A) 6))
(assert (= (set.card B) 5))
(assert (= (set.inter A B) (as set.empty (Set Int))))
(assert (= universe (as set.universe (Set Int))))
(check-sat)
