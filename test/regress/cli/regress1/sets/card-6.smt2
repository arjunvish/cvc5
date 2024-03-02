;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_UFLIAFS)
(set-info :status unsat)
(declare-sort E 0)
(declare-fun A () (Set E))
(declare-fun B () (Set E))
(declare-fun C () (Set E))
(assert
  (and
    (= (as set.empty (Set E))
       (set.inter A B))
    (set.subset C (set.union A B))
    (>= (set.card C) 5)
    (<= (set.card A) 2)
    (<= (set.card B) 2)
  )
)
(check-sat)
