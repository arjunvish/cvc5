;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-info :status unsat)
(set-logic QF_BV)
(declare-fun x () (_ BitVec 16))
(declare-fun y () (_ BitVec 12))
(assert (= y ((_ extract 11 0) x)))
(assert (= y ((_ extract 15 4) x)))
(assert (= ((_ extract 3 2) y) ((_ extract 1 0) y)))
(assert (= ((_ extract 1 0) x) (_ bv1 2)))
(check-sat-assuming ( (not (= x (_ bv21845 16))) ))
