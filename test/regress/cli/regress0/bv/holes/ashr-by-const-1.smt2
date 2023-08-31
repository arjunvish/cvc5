; EXPECT: unsat
(set-info :smt-lib-version 2.6)
(set-logic QF_BV)
(set-info :status unsat)

(declare-const x (_ BitVec 5))
(assert (not (=
  (bvashr x #b00011)
  (concat
    ((_ repeat 3) ((_ extract 4 4) x))
    ((_ extract 4 3) x))
	)))
(check-sat)
(exit)
