;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun st3 () (Set String))
(declare-fun st9 () (Set String))
(assert (set.is_singleton (set.inter st3 st9)))
(assert (< 1 (set.card (set.inter st3 st9))))
(assert (set.is_singleton st9))
(check-sat)
