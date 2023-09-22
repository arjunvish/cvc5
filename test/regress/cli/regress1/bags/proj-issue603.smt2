(set-logic ALL)
(set-info :status sat)
(set-option :check-unsat-cores true)
(set-option :proof-prune-input true)
(declare-const x Int)
(assert (bag.subbag (bag 0 1) (bag 0 (bag.card (bag.union_disjoint (bag 0 1) (bag x 1))))))
(check-sat)
