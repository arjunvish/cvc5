; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-logic ALL)

(declare-fun x () (Relation Int Int))
(declare-fun y () (Relation Int Int))
(declare-fun a () Int)
(declare-fun z () (Tuple Int Int))
(assert (= z (tuple 1 2)))
(declare-fun zt () (Tuple Int Int))
(assert (= zt (tuple 2 1)))
(assert (set.member z x))
(assert (not (set.member zt (rel.transpose x))))
(assert (= y (rel.transpose x)))
(check-sat)
