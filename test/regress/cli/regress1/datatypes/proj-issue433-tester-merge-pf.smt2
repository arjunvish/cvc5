;; Datatypes are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-datatypes ((L 0)) (((i))))
(declare-datatypes ((O 0)) (((N) (S (i1 (_ BitVec 3))))))
(declare-fun m (L) O)
(declare-fun g ((_ BitVec 3) L) L)
(assert (exists ((B L) (n (_ BitVec 3))) (not (ite ((_ is S) (m (g n B))) (not ((_ is N) (m (g n B)))) true))))
(check-sat)
