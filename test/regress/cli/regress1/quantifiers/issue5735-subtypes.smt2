;; introduces div_by_zero Skolem
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun a () Bool)
(assert (forall ((b Int) (c Bool) (d Int))
(or (= (to_real d) (/ 1.0 (ite c 9.0 0.0))) (<= (ite a 1.0 (to_real b)) (/ 1.0 (ite c 9.0 0.0))))))
(check-sat)
