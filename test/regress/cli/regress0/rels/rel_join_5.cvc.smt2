; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-logic ALL)

(declare-fun x () (Relation Int Int))
(declare-fun y () (Relation Int Int))
(declare-fun z () (Relation Int Int))
(declare-fun r () (Relation Int Int))
(assert (set.member (tuple 7 1) x))
(assert (set.member (tuple 2 3) x))
(assert (set.member (tuple 7 3) y))
(assert (set.member (tuple 4 7) y))
(assert (set.member (tuple 3 4) z))
(declare-fun a () (Tuple Int Int))
(assert (= a (tuple 1 4)))
(assert (= r (rel.join (rel.join (rel.transpose x) y) z)))
(assert (not (set.member a r)))
(check-sat)
