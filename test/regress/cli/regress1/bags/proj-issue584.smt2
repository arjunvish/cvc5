;; Bags are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-const x (Bag Bool))
(assert (> (bag.card (bag.inter_min x bag.empty)) (bag.card (bag.inter_min x bag.empty))))
(check-sat)
