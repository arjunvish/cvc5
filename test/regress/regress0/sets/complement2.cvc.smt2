; EXPECT: unsat
(set-option :incremental false)
(set-option :sets-ext true)
(set-logic ALL)
(declare-sort Atom 0)
(declare-fun a () (Set Atom))
(declare-fun b () (Set Atom))
(declare-fun c () Atom)
(assert (= a (complement a)))
(assert (member c a))
(check-sat)
