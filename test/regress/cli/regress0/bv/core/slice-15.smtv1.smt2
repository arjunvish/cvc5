;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-info :status unsat)
(set-logic QF_BV)
(declare-fun x () (_ BitVec 16))
(assert (= ((_ extract 15 15) x) (_ bv1 1)))
(assert (= ((_ extract 15 1) x) ((_ extract 14 0) x)))
(check-sat-assuming ( (not (= x (_ bv65535 16))) ))
