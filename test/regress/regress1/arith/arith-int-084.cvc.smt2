; EXPECT: sat
(set-logic ALL)
(set-option :incremental false)
(declare-fun x0 () Int)
(declare-fun x1 () Int)
(declare-fun x2 () Int)
(declare-fun x3 () Int)
(assert (<= (+ (+ (+ (* (- 2) x0) (* (- 13) x1)) (* (- 14) x2)) (* (- 26) x3)) 4))
(assert (let ((_let_1 (- 17))) (< (+ (+ (+ (* _let_1 x0) (* _let_1 x1)) (* 21 x2)) (* (- 4) x3)) 18)))
(assert (> (+ (+ (+ (* (- 31) x0) (* 23 x1)) (* 4 x2)) (* 29 x3)) (- 6)))
(assert (let ((_let_1 (- 8))) (<= (+ (+ (+ (* (- 14) x0) (* 32 x1)) (* _let_1 x2)) (* _let_1 x3)) (- 1))))
(check-sat-assuming ( (not false) ))
