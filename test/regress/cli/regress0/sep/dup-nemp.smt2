;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_ALL)
(set-info :status unsat)
(declare-sort Loc 0)
(declare-const l Loc)
(declare-heap (Loc Loc))
(assert (sep (not sep.emp) (not sep.emp)))
(assert (pto l l))
(check-sat)
