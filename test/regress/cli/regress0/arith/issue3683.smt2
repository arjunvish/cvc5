;; unsupported symbol exp
; DISABLE-TESTER: alethe
(set-logic ALL)
(declare-fun a () Real)
(assert (= (+ 2.0 (exp (+ 2.0 a))) 0.0))
(set-info :status unsat)
(check-sat)
