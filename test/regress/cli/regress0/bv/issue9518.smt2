;; FP is not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(set-option :bv-solver bitblast-internal)
(set-option :check-models true)
(declare-const v (_ BitVec 1))
(declare-fun a () (Array (_ BitVec 32) (_ BitVec 32)))
(assert (forall ((V (_ FloatingPoint 2 3))) (= (store a (_ bv0 32) (_ bv0 32)) (store a (_ bv0 32) (bvadd (_ bv1 32) ((_ zero_extend 31) v))))))
(check-sat)