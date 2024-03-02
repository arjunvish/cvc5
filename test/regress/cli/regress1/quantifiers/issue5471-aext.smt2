;; introduces div_by_zero Skolem
; DISABLE-TESTER: alethe
(set-logic NRA)
(set-option :sygus-inst true)
(set-option :strings-exp true)
(set-info :status unsat)
(declare-fun a () Real)
(declare-fun b () Real)
(declare-fun c () Real)
(assert (forall ((d Real)) (= (> d 0) (<= (+ d (/ a c)) 0))))
(assert (<= (* b b) 0))
(check-sat)
