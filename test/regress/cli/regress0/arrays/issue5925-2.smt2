; DISABLE-TESTER: proof
(set-logic QF_A)
(set-info :status unsat)
(declare-const a (Array Bool Bool))
(assert (= (store a true false) (store (store ((as const (Array Bool Bool)) true) true false) false false)))
(assert (select a false))
(check-sat)