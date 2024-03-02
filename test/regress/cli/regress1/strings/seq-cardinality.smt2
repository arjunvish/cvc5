;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun x () (Seq (_ BitVec 1)))
(declare-fun y () (Seq (_ BitVec 1)))
(declare-fun z () (Seq (_ BitVec 1)))

(assert (= (seq.len x) 1))
(assert (= (seq.len y) 1))
(assert (= (seq.len z) 1))
(assert (distinct x y z))
(check-sat)
