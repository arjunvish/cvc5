; DISABLE-TESTER: lfsc
;; Nullables are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic HO_ALL)
(set-info :status unsat)
(assert (= (nullable.lift (lambda ((x Bool) (y Bool)) (and x y)) (nullable.some false) (nullable.some false)) (nullable.some true)))
(check-sat)
