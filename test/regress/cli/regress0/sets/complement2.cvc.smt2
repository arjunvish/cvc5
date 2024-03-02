; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-option :sets-ext true)
(set-logic ALL)
(declare-sort Atom 0)
(declare-fun a () (Set Atom))
(declare-fun b () (Set Atom))
(declare-fun c () Atom)
(assert (= a (set.complement a)))
(assert (set.member c a))
(check-sat)
