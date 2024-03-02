; COMMAND-LINE: --bv-intro-pow2
; DISABLE-TESTER: unsat-core
;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-info :smt-lib-version 2.6)
(set-logic QF_BV)
(set-info :status unsat)
(declare-fun x () (_ BitVec 1024))
(declare-fun y () (_ BitVec 1024))
(declare-fun z () (_ BitVec 1024))
(assert (= z (bvadd x y)))
(assert (distinct x y))
(assert (and (distinct x (_ bv0 1024)) (= (bvand x (bvsub x (_ bv1 1024))) (_ bv0 1024))))
(assert (and (distinct y (_ bv0 1024)) (= (bvand y (bvsub y (_ bv1 1024))) (_ bv0 1024))))
(assert (and (distinct z (_ bv0 1024)) (= (bvand z (bvsub z (_ bv1 1024))) (_ bv0 1024))))
(check-sat)
(exit)
