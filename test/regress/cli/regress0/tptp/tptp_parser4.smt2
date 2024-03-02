; COMMAND-LINE: --finite-model-find
;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-sort $$unsorted 0)
(declare-fun tptp.d () $$unsorted)
(declare-fun tptp.p ($$unsorted $$unsorted) Bool)
(declare-fun tptp.b () Bool)
(assert (forall ((A $$unsorted)) (or (tptp.p A tptp.d) tptp.b)))
(declare-fun tptp.c () Bool)
(assert (or tptp.b tptp.c))
(assert (forall ((A $$unsorted)) (or (not (tptp.p A tptp.d)) (not tptp.c))))
(assert (not tptp.b))
(set-info :filename tptp_parser4)
(check-sat)
