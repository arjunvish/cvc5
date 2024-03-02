; EXPECT: unsat
;; HO not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic HO_UF)
(set-info :status unsat)
(declare-sort U 0)
(declare-fun f (U U) U)
(declare-fun g (U) U)
(declare-fun a () U)
(declare-fun b () U)
(declare-fun c () U)
(declare-fun d () U)
(assert (or (= g (f a)) (= g (f b))))
(assert (not (= (f a a) b)))
(assert (not (= (f b a) b)))
(assert (not (= (f a a) c)))
(assert (not (= (f b a) c)))
(assert (or (= (g a) b) (= (g a) c)))
(check-sat)
