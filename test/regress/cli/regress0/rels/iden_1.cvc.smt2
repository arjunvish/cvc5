; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-option :sets-ext true)
(set-logic ALL)
(declare-sort Atom 0)

(declare-fun x () (Relation Atom Atom))
(declare-fun t () (Relation Atom))
(declare-fun univ () (Relation Atom))
(declare-fun a () Atom)
(declare-fun b () Atom)
(declare-fun c () Atom)
(declare-fun d () Atom)
(assert (= univ (as set.universe (Relation Atom))))
(assert (set.member (tuple a b) x))
(assert (set.member (tuple c d) x))
(assert (not (= a b)))
(assert (= (rel.iden (rel.join x univ)) x))
(check-sat)
