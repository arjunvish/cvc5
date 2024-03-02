;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_SLIA)
(set-info :status unsat)
(declare-fun x () (Seq Int))
(declare-fun y () (Seq Int))
(declare-fun z () Int)
(assert (> z 10))
(assert (= (seq.len x) (seq.len y)))
(assert (= (seq.++ x (seq.unit z)) (seq.++ y (seq.unit 5))))
(check-sat)
