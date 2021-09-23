; EXPECT: unsat
(set-option :incremental false)
(set-logic ALL)


(declare-fun x () (Set (Tuple Int Int)))
(declare-fun y () (Set (Tuple Int Int)))
(declare-fun z () (Set (Tuple Int Int)))
(declare-fun r () (Set (Tuple Int Int Int Int)))
(assert (member (mkTuple 2 1) x))
(assert (member (mkTuple 2 3) x))
(assert (member (mkTuple 2 2) y))
(assert (member (mkTuple 4 7) y))
(assert (member (mkTuple 3 4) z))
(declare-fun v () (Tuple Int Int Int Int))
(assert (= v (mkTuple 4 3 2 1)))
(assert (= r (product (join (transpose x) y) z)))
(assert (not (member v (transpose r))))
(check-sat)
