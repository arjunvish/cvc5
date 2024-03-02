; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-logic ALL)

(declare-fun w () (Relation Int Int))
(declare-fun x () (Relation Int Int))
(declare-fun y () (Relation Int Int))
(declare-fun z () (Relation Int Int))
(declare-fun r () (Relation Int Int))
(declare-fun t () Int)
(declare-fun u () Int)
(assert (and (< 4 t) (< t 6)))
(assert (and (< 4 u) (< u 6)))
(assert (set.member (tuple 1 u) x))
(assert (set.member (tuple t 3) y))
(declare-fun a () (Tuple Int Int))
(assert (= a (tuple 1 3)))
(assert (not (set.member a (rel.join x y))))
(check-sat)
