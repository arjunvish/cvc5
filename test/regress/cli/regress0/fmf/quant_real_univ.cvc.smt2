; EXPECT: sat
(set-logic ALL)
(set-option :incremental false)
(set-option :fmf-bound true)
(set-option :sets-ext true)
(declare-sort Atom 0)
(declare-fun REAL_UNIVERSE () (Relation Real))
(declare-fun ATOM_UNIVERSE () (Relation Atom))
(assert (= REAL_UNIVERSE (as set.universe (Relation Real))))
(assert (= ATOM_UNIVERSE (as set.universe (Relation Atom))))
(declare-fun levelVal () (Relation Atom Real))
(assert (forall ((s Atom) (v1 Real) (v2 Real)) (=> (and (and (set.member (tuple s) ATOM_UNIVERSE) (set.member (tuple v1) REAL_UNIVERSE)) (set.member (tuple v2) REAL_UNIVERSE)) (=> (and (set.member (tuple s v1) levelVal) (set.member (tuple s v2) levelVal)) (= v1 v2)))))
(check-sat)
