; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-logic ALL)

(declare-fun x () (Relation Int Int))
(declare-fun y () (Relation Int Int))
(declare-fun r () (Relation Int Int))
(declare-fun z () (Tuple Int Int))
(assert (= z (tuple 1 2)))
(declare-fun zt () (Tuple Int Int))
(assert (= zt (tuple 2 1)))
(assert (set.member zt y))
(declare-fun w () (Tuple Int Int))
(assert (= w (tuple 2 2)))
(assert (set.member w y))
(assert (set.member z x))
(assert (not (set.member zt (rel.transpose (rel.join x y)))))
(check-sat)
