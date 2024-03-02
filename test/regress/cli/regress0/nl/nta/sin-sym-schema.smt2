;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun a () Real)
(declare-fun b () Real)
(declare-fun c () Real)
(assert (<= 0.0 a 2.0))
(assert (or (= (+ a c) 0.0) (= (+ a b) 0.0)))
(assert (not (= (sin a) (- (sin b)))))
(assert (not (= (sin a) (- (sin c)))))
(check-sat)
