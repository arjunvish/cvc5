; COMMAND-LINE: -q
; EXPECT: sat
(set-logic QF_UFNRA)
(set-info :status sat)
(declare-fun ufrr5 (Real Real Real Real Real) Real)
(declare-fun r5 () Real)
(declare-fun r6 () Real)
(declare-fun r37 () Real)
(declare-fun r58 () Real)
(assert (and (> 0.0 r37) (> r37 (ufrr5 0.0 1.0 1.0 r5 1.0)) (= 0.0 (+ (- 1) (* r6 r58 (- 1))))))
(assert (= 0.0 (/ 1.0 r5)))
(check-sat)
