;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(set-option :sets-ext true)
(declare-const x Bool)
(declare-const x3 Bool)
(declare-const x4 Bool)
(assert (forall ((x2 Bool)) (ite (not (set.choose (set.complement (set.insert x3 x4 false false (set.singleton x))))) (set.choose (set.complement (set.insert x3 x4 x2 false (set.singleton x)))) false)))
(check-sat)
