;; Datatypes are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-datatypes ((@ 0)) (((V))))
(declare-fun I (@ Int) Bool)
(assert (forall ((v @) (i Int)) (! false :qid |outputbpl.122:24| :pattern ((I v i)))))
(check-sat)
