;; unsupported eqrange operator
; DISABLE-TESTER: alethe
(set-logic AUFBV)
(set-option :arrays-exp true)
(set-info :status unsat)
(declare-sort Element 0)
(declare-const a (Array (_ BitVec 3) Element))
(declare-const b (Array (_ BitVec 3) Element))
(declare-const j (_ BitVec 3))
(assert (eqrange a b (_ bv0 3) j))
(assert (eqrange a b (bvadd j (_ bv1 3))  (_ bv7 3)))
(assert (exists ((i (_ BitVec 3))) (not (= (select a i) (select b i)))))
(check-sat)
(exit)
