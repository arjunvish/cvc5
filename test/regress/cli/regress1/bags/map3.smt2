;; Bags are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic HO_ALL)
(set-info :status unsat)
(set-option :fmf-bound true)
(declare-fun A () (Bag Int))
(declare-fun B () (Bag Int))
(define-fun f ((x Int)) Int (+ x 1))
(assert (= B (bag.map f A)))
(assert (= (bag.count (- 2) B) 57))
(assert (= A (as bag.empty (Bag Int)) ))
(check-sat)
