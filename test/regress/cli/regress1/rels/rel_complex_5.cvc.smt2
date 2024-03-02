; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental true)
(set-logic ALL)


(declare-fun x () (Relation Int Int))
(declare-fun y () (Relation Int Int))
(declare-fun z () (Relation Int Int))
(declare-fun r () (Relation Int Int))
(declare-fun w () (Relation Int Int))
(declare-fun f () (Tuple Int Int))
(assert (= f (tuple 3 1)))
(assert (set.member f x))
(declare-fun g () (Tuple Int Int))
(assert (= g (tuple 1 3)))
(assert (set.member g y))
(declare-fun h () (Tuple Int Int))
(assert (= h (tuple 3 5)))
(assert (set.member h x))
(assert (set.member h y))
(assert (= r (rel.join x y)))
(declare-fun a () (Tuple Int))
(assert (= a (tuple 1)))
(declare-fun e () (Tuple Int Int))
(assert (= e (tuple 1 1)))
(assert (let ((_let_1 (set.singleton a))) (= w (rel.product _let_1 _let_1))))
(assert (set.subset (rel.transpose w) y))
(assert (not (set.member e r)))
(assert (not (= z (set.inter x y))))
(assert (= z (set.minus x y)))
(assert (set.subset x y))
(assert (set.member e (rel.join r z)))
(assert (set.member e x))
(assert (set.member e (set.inter x y)))
(push 1)

(assert true)

(check-sat)

(pop 1)
