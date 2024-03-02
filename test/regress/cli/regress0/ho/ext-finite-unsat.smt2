; EXPECT: unsat
;; HO not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic HO_UF)
(set-info :status unsat)
(declare-sort U 0)
(declare-fun f (U) U)
(declare-fun g (U) U)
(declare-fun h (U) U)
(declare-fun a () U)
(declare-fun b () U)
(declare-fun c () U)
(assert (distinct f g h))
(assert (forall ((x U) (y U)) (= x y)))
(check-sat)
