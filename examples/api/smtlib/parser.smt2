(set-logic QF_LIA)
(declare-fun a () Int)
(declare-fun b () Int)
(declare-fun c () Int)
(assert (> a (+ b c)))
(assert (< a b))
(assert (> c 0))
(check-sat)
