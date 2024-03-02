;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-logic HO_ALL)
(set-info :status unsat)
(declare-sort $$unsorted 0)
(declare-fun tptp.g (Bool) Bool)
(declare-fun tptp.p ((-> Bool Bool)) Bool)
(declare-fun tptp.x () Bool)
(declare-fun tptp.y () Bool)
(assert (and (not (= tptp.x tptp.y)) (= (@ tptp.g tptp.x) tptp.y) (= (@ tptp.g tptp.y) tptp.x) (@ tptp.p tptp.g) (not (@ tptp.p (lambda ((_lvar_0 Bool)) (not _lvar_0))))))
(set-info :filename bug_nodbuilding_interpreted_SYO042^1)
(check-sat)
