;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-info :status unsat)
(set-logic QF_BV)
(declare-fun x () (_ BitVec 8))
(assert (= ((_ extract 3 0) x) (_ bv0 4)))
(assert (= ((_ extract 7 4) x) (_ bv15 4)))
(check-sat-assuming ( (not (= x (_ bv240 8))) ))
