;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_UFLIAFS)
(set-info :status unsat)
(declare-sort E 0)
(declare-fun s () (Set E))
(declare-fun t () (Set E))
(assert (>= (set.card s) 5))
(assert (>= (set.card t) 5))
(assert (<= (set.card (set.union s t)) 4))
(check-sat)
