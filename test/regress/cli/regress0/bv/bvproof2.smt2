;; introduces arrays Skolem
; DISABLE-TESTER: alethe
(set-logic QF_ABV)
(set-info :status unsat)
(declare-const v (_ BitVec 1))
(declare-const _v (_ BitVec 2))
(declare-fun a () (Array (_ BitVec 3) (_ BitVec 2)))
(assert (not (= (store (store (store a ((_ zero_extend 1) _v) (_ bv0 2)) (_ bv1 3) (_ bv0 2)) (_ bv0 3) (_ bv0 2)) (store (store (store (store a (_ bv0 3) (_ bv0 2)) ((_ zero_extend 2) v) (_ bv0 2)) (_ bv1 3) (_ bv0 2)) ((_ zero_extend 1) _v) (_ bv0 2)))))
(check-sat)
