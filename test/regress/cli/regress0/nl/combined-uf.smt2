; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_UFNRA)
(declare-fun a () Real)
(declare-fun b () Real)
(declare-fun f (Real) Real)
(assert (= (* a a) 2))
(assert (> a 0))
(assert (= (* b b b b) 4))
(assert (< b 0))
(assert (not (= (f (* a a)) (f (* b b)))))
(check-sat)
