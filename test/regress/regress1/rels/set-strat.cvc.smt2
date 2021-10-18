; EXPECT: sat
(set-option :incremental false)
(set-logic ALL)


(declare-fun x () (Set (Tuple Int Int)))
(declare-fun y () (Set (Tuple Int Int)))
(declare-fun w () (Set (Tuple Int Int)))
(declare-fun z () (Set (Tuple (Set (Tuple Int Int)) (Set (Tuple Int Int)))))
(declare-fun a () (Tuple Int Int))
(declare-fun b () (Tuple Int Int))
(assert (not (= a b)))
(assert (member a x))
(assert (member b y))
(assert (member b w))
(assert (member (tuple x y) z))
(assert (member (tuple w x) z))
(assert (not (member (tuple x x) (join z z))))
(assert (member (tuple x (singleton (tuple 0 0))) (join z z)))
(check-sat)
