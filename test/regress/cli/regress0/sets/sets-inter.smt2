;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(define-sort SetInt () (Set Int))
(declare-fun a () (Set Int))
(declare-fun b () (Set Int))
(declare-fun x () Int)
;(assert (not (set.member x a)))
(assert (set.member x (set.inter a b)))
(assert (not (set.member x b)))
(check-sat)
(exit)
