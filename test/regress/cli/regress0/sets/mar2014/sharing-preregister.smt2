; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_UFLIAFS)
(declare-fun a () Int)
(declare-fun b () Int)
(declare-fun x () (Set Int))
(declare-fun y () (Set Int))
(assert (= x (set.singleton a)))
(assert (= y (set.singleton b)))
(assert (not (= x y)))
(assert (and (< 1 a) (< a 3) (< 1 b) (< b 3)))
(check-sat)
