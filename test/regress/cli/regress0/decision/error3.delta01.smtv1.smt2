;; unsupported bitblasting of bvshl
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-info :status unsat)
(set-logic QF_AUFBV)
(declare-fun v1 () (_ BitVec 3))
(declare-fun a2 () (Array (_ BitVec 13) (_ BitVec 3)))
(check-sat-assuming ( (let ((_let_0 (bvnot (ite (bvslt ((_ sign_extend 2) (ite (bvsgt v1 v1) (_ bv1 1) (_ bv0 1))) v1) (_ bv1 1) (_ bv0 1))))) (and (not (= (_ bv0 3) (bvshl ((_ sign_extend 2) (ite (bvslt ((_ sign_extend 2) (ite (bvsgt v1 v1) (_ bv1 1) (_ bv0 1))) v1) (_ bv1 1) (_ bv0 1))) (select a2 (_ bv0 13))))) (and (not (= ((_ zero_extend 10) _let_0) (_ bv0 11))) (bvult (_ bv0 11) ((_ zero_extend 9) ((_ repeat 2) _let_0)))))) ))
