; COMMAND-LINE: --strings-exp
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_UFSLIA)
(set-info :status unsat)
(declare-fun a () (Seq Int))
(declare-fun b () (Seq Int))
(declare-fun c () (Seq Int))
(declare-fun x () Int)
(declare-fun y () Int)
(assert (or (= a b) (= a c)))
(assert (not (= (seq.nth a x) (seq.nth b x))))
(assert (not (= (seq.nth a y) (seq.nth c y))))
(check-sat)
