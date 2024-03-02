;; Bags are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic HO_ALL)
(set-info :status unsat)
(set-option :fmf-bound true)
(declare-fun A () (Bag Int))
(declare-fun B () (Bag Int))
(define-fun p ((x Int)) Bool (> x 1))
(assert (= B (bag.filter p A)))
(assert (= (bag.count 3 A) 57))
(assert (= (bag.count 3 B) 58))
(check-sat)
