;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic HO_ALL)
(set-info :status unsat)
(declare-fun A () (Set Int))
(declare-fun B () (Set Int))
(define-fun f ((x Int)) Int (+ x 1))
(assert (= B (set.map f A)))
(assert (set.member (- 2) B))
(assert (= A (as set.empty (Set Int))))
(check-sat)
