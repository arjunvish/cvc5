; EXPECT: sat
(set-option :incremental false)
(set-logic ALL)


(declare-fun x () (Set (Tuple Int Int)))
(declare-fun y () (Set (Tuple Int Int)))
(declare-fun r () (Set (Tuple Int Int)))
(declare-fun w () (Set (Tuple Int)))
(declare-fun z () (Set (Tuple Int)))
(declare-fun r2 () (Set (Tuple Int Int)))
(declare-fun d () (Tuple Int Int))
(assert (= d (tuple 1 3)))
(assert (member (tuple 1 3) y))
(declare-fun a () (Tuple Int Int))
(assert (member a x))
(declare-fun e () (Tuple Int Int))
(assert (= e (tuple 4 3)))
(assert (= r (join x y)))
(assert (= r2 (product w z)))
(assert (not (member e r)))
(assert (not (= r r2)))
(check-sat)
