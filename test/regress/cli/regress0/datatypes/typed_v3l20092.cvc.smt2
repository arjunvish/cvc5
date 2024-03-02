; EXPECT: unsat
;; Datatypes are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-option :incremental false)
(declare-datatypes ((nat 0)(list 0)(tree 0)) (((succ (pred nat)) (zero))((cons (car tree) (cdr list)) (null))((node (children list)) (leaf (data nat)))))
(declare-fun x1 () nat)
(declare-fun x2 () nat)
(declare-fun x3 () nat)
(declare-fun x4 () list)
(declare-fun x5 () list)
(declare-fun x6 () list)
(declare-fun x7 () tree)
(declare-fun x8 () tree)
(declare-fun x9 () tree)
(check-sat-assuming ( (not (let ((_let_1 (node (ite ((_ is cons) x6) (cdr x6) null)))) (let ((_let_2 (ite ((_ is cons) null) (car null) (leaf zero)))) (not (and (= (ite ((_ is succ) x1) (pred x1) zero) (ite ((_ is leaf) _let_2) (data _let_2) zero)) (= (cons (node x6) (ite ((_ is node) x7) (children x7) null)) (ite ((_ is node) _let_1) (children _let_1) null))))))) ))
