;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(assert (set.member 5 (as set.empty (Set Int) )))
(check-sat)
