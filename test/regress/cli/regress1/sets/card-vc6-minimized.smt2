; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_UFLIAFS)
(declare-fun x () Int)
(declare-fun c () (Set Int))
(declare-fun alloc0 () (Set Int))
(declare-fun alloc1 () (Set Int))
(declare-fun alloc2 () (Set Int))
(assert
(and (set.member x c)
      (<= (set.card (set.minus alloc1 alloc0)) 1)
      (<= (set.card (set.minus alloc2 alloc1))
          (set.card (set.minus c (set.singleton x))))
      (> (set.card (set.minus alloc2 alloc0)) (set.card c))
))
(check-sat)
