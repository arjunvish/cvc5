; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-logic ALL)

(declare-fun x () (Relation Int Int))
(declare-fun y () (Relation Int Int))
(declare-fun z () (Relation Int Int))
(declare-fun r () (Relation Int Int))
(assert (= x (set.union (set.singleton (tuple 1 7)) (set.singleton (tuple 2 3)))))
(declare-fun d () (Tuple Int Int))
(assert (= d (tuple 7 3)))
(declare-fun e () (Tuple Int Int))
(assert (= e (tuple 4 7)))
(assert (set.member d y))
(assert (set.member e y))
(assert (set.member (tuple 3 4) z))
(declare-fun a () (Tuple Int Int))
(assert (= a (tuple 4 1)))
(assert (= r (rel.join (rel.join x y) z)))
(assert (not (set.member a (rel.transpose r))))
(check-sat)
