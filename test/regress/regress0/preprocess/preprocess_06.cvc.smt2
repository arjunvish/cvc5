; EXPECT: unsat
(set-logic ALL)
(set-option :incremental false)
(declare-fun a0 () Bool)
(declare-fun a1 () Bool)
(declare-fun a2 () Bool)
(declare-fun a3 () Bool)
(declare-fun a4 () Bool)
(declare-fun a5 () Bool)
(assert (=> a0 a1))
(assert (=> a1 a2))
(assert (=> a2 a3))
(assert (=> a3 a4))
(assert (=> a4 a5))
(assert a0)
(check-sat-assuming ( (not (= a0 a5)) ))
