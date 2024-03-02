; COMMAND-LINE: --mbqi --ho-elim
; EXPECT: unsat
;; HO not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic HO_ALL)
(declare-sort $$unsorted 0)
(declare-fun tptp.eps ((-> $$unsorted Bool)) $$unsorted)
(assert (forall ((P (-> $$unsorted Bool))) (=> (exists ((X $$unsorted)) (@ P X)) (@ P (@ tptp.eps P)))))
(declare-fun tptp.epsio ((-> (-> $$unsorted Bool) Bool) $$unsorted) Bool)
(assert (forall ((P (-> (-> $$unsorted Bool) Bool))) (=> (exists ((X (-> $$unsorted Bool))) (@ P X)) (@ P (@ tptp.epsio P)))))
(declare-fun tptp.setunion ((-> (-> $$unsorted Bool) Bool) $$unsorted) Bool)
(assert (= tptp.setunion (lambda ((C (-> (-> $$unsorted Bool) Bool)) (X $$unsorted)) (exists ((Y (-> $$unsorted Bool))) (and (@ C Y) (@ Y X))))))
(declare-fun tptp.choosenonempty ((-> (-> $$unsorted Bool) Bool) $$unsorted) Bool)
(assert (= tptp.choosenonempty (lambda ((C (-> (-> $$unsorted Bool) Bool)) (__flatten_var_0 $$unsorted)) (@ (@ tptp.epsio (lambda ((Y (-> $$unsorted Bool))) (and (@ C Y) (@ Y (@ tptp.eps Y))))) __flatten_var_0))))
(declare-fun tptp.c ((-> $$unsorted Bool)) Bool)
(declare-fun tptp.a () $$unsorted)
(assert (@ (@ tptp.setunion tptp.c) tptp.a))
(assert (not (and (@ tptp.c (@ tptp.choosenonempty tptp.c)) (exists ((X $$unsorted)) (@ (@ tptp.choosenonempty tptp.c) X)))))
(set-info :filename SEV428^1)
(check-sat-assuming ( true ))
