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
(declare-fun v () (Tuple Int Int))
(assert (= v (tuple 1 1)))
(declare-fun a () (Tuple Int Int))
(assert (= a (tuple 1 5)))
(assert (set.member (tuple 1 7) x))
(assert (set.member (tuple 7 5) y))
(assert (set.member z x))
(assert (set.member zt y))
(assert (not (set.member a (rel.join x y))))
(check-sat)
