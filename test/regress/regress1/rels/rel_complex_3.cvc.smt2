; EXPECT: unsat
(set-option :incremental true)
(set-logic ALL)

(declare-fun x () (Set (Tuple Int Int)))
(declare-fun y () (Set (Tuple Int Int)))
(declare-fun z () (Set (Tuple Int Int)))
(declare-fun r () (Set (Tuple Int Int)))
(declare-fun w () (Set (Tuple Int Int)))
(declare-fun f () (Tuple Int Int))
(assert (= f (tuple 3 1)))
(assert (member f x))
(declare-fun g () (Tuple Int Int))
(assert (= g (tuple 1 3)))
(assert (member g y))
(declare-fun h () (Tuple Int Int))
(assert (= h (tuple 3 5)))
(assert (member h x))
(assert (member h y))
(assert (= r (join x y)))
(declare-fun e () (Tuple Int Int))
(assert (not (member e r)))
(assert (not (= z (intersection x y))))
(assert (= z (setminus x y)))
(assert (subset x y))
(assert (member e (join r z)))
(assert (member e x))
(assert (member e (intersection x y)))
(push 1)

(assert true)

(check-sat)

(pop 1)

