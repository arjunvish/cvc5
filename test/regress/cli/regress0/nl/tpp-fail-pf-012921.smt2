;; introduces div_by_zero Skolem
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun x () Real)
(assert (and (> 0.0 x) (not (= 0.0 (/ 0.0 (* 2.0 x))))))
(check-sat)
