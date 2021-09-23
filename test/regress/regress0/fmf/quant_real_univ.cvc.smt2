; EXPECT: sat
(set-logic ALL)
(set-option :incremental false)
(set-option :fmf-bound true)
(set-option :sets-ext true)
(declare-sort Atom 0)
(declare-fun REAL_UNIVERSE () (Set (Tuple Real)))
(declare-fun ATOM_UNIVERSE () (Set (Tuple Atom)))
(assert (= REAL_UNIVERSE (as univset (Set (Tuple Real)))))
(assert (= ATOM_UNIVERSE (as univset (Set (Tuple Atom)))))
(declare-fun levelVal () (Set (Tuple Atom Real)))
(assert (forall ((s Atom) (v1 Real) (v2 Real)) (=> (and (and (member (mkTuple s) ATOM_UNIVERSE) (member (mkTuple v1) REAL_UNIVERSE)) (member (mkTuple v2) REAL_UNIVERSE)) (=> (and (member (mkTuple s v1) levelVal) (member (mkTuple s v2) levelVal)) (= v1 v2)))))
(check-sat)
