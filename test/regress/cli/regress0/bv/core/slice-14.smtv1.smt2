;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-info :status unsat)
(set-logic QF_BV)
(declare-fun x () (_ BitVec 6))
(assert (= ((_ extract 5 1) x) ((_ extract 4 0) x)))
(assert (= ((_ extract 0 0) x) (_ bv0 1)))
(check-sat-assuming ( (not (= x (_ bv0 6))) ))
