; EXPECT: sat
(set-logic ALL)
(set-option :incremental false)
(declare-fun x0 () Int)
(declare-fun x1 () Int)
(declare-fun x2 () Int)
(declare-fun x3 () Int)
(assert (= (+ (+ (+ (* (- 13) x0) (* (- 11) x1)) (* (- 14) x2)) (* 21 x3)) 6))
(assert (<= (+ (+ (+ (* 7 x0) (* 5 x1)) (* 13 x2)) (* 21 x3)) 27))
(assert (< (+ (+ (+ (* 15 x0) (* (- 11) x1)) (* (- 19) x2)) (* (- 13) x3)) 5))
(check-sat-assuming ( (not false) ))
