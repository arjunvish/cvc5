; EXPECT: sat
(set-logic ALL)
(set-option :incremental false)
(declare-fun x0 () Int)
(declare-fun x1 () Int)
(declare-fun x2 () Int)
(declare-fun x3 () Int)
(assert (> (+ (+ (+ (* (- 20) x0) (* (- 19) x1)) (* 6 x2)) (* 32 x3)) 16))
(assert (< (+ (+ (+ (* (- 1) x0) (* (- 30) x1)) (* 15 x2)) (* 7 x3)) (- 10)))
(assert (< (+ (+ (+ (* (- 13) x0) (* 24 x1)) (* 27 x2)) (* 20 x3)) (- 5)))
(check-sat-assuming ( (not false) ))
