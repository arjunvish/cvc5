; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-logic ALL)

(declare-fun x () (Relation Int Int))
(declare-fun y () (Relation Int Int))
(declare-fun a () Int)
(declare-fun b () Int)
(declare-fun c () Int)
(declare-fun d () Int)
(assert (set.member (tuple 1 a) x))
(assert (set.member (tuple 1 c) x))
(assert (set.member (tuple 1 d) x))
(assert (set.member (tuple b 1) x))
(assert (= b d))
(assert (> b (- d 1)))
(assert (< b (+ d 1)))
(assert (= y (rel.tclosure x)))
(assert (not (set.member (tuple 1 1) y)))
(check-sat)
