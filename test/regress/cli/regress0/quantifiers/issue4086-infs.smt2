; DISABLE-TESTER: alethe
(set-logic LIRA)
(set-info :status unsat)
(set-option :cegqi-inf-int true)
(set-option :cegqi-inf-real true)
(set-option :var-ineq-elim-quant false)
(assert (forall (( b Real )) (forall (( c Int )) (and  (> (to_real c) (* b 2.0))))))
(check-sat)
