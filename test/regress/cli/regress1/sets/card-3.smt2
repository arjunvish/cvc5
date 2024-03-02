;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_UFLIAFS)
(set-info :status unsat)
(declare-sort E 0)
(declare-fun s () (Set E))
(declare-fun t () (Set E))
(declare-fun u () (Set E))
(assert (>= (set.card (set.union s t)) 8))
(assert (>= (set.card (set.union s u)) 8))
(assert (<= (set.card (set.union t u)) 5))
(assert (<= (set.card s) 5))
(assert (= (as set.empty (Set E)) (set.inter t u)))
(check-sat)
