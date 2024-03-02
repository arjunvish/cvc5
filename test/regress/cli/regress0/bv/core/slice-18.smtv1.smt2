;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-info :status unsat)
(set-logic QF_BV)
(declare-fun x () (_ BitVec 16))
(declare-fun y () (_ BitVec 12))
(assert (= ((_ extract 0 0) x) (_ bv1 1)))
(assert (= y ((_ extract 11 0) x)))
(assert (= y ((_ extract 15 4) x)))
(assert (= ((_ extract 3 1) y) ((_ extract 2 0) y)))
(check-sat-assuming ( (not (= x (_ bv65535 16))) ))
