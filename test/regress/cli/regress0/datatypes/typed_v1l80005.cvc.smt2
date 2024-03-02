; EXPECT: unsat
;; Datatypes are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-option :incremental false)
(declare-datatypes ((nat 0)(list 0)(tree 0)) (((succ (pred nat)) (zero))((cons (car tree) (cdr list)) (null))((node (children list)) (leaf (data nat)))))
(declare-fun x1 () nat)
(declare-fun x2 () list)
(declare-fun x3 () tree)
(check-sat-assuming ( (not (let ((_let_1 ((_ is cons) null))) (let ((_let_2 (ite _let_1 (cdr null) null))) (let ((_let_3 (node (cons (leaf (succ zero)) null)))) (let ((_let_4 (leaf zero))) (let ((_let_5 (succ (ite ((_ is leaf) x3) (data x3) zero)))) (let ((_let_6 (leaf (ite ((_ is succ) _let_5) (pred _let_5) zero)))) (let ((_let_7 (ite ((_ is succ) zero) (pred zero) zero))) (let ((_let_8 (succ (ite ((_ is succ) _let_7) (pred _let_7) zero)))) (let ((_let_9 (cons (node null) (ite ((_ is cons) x2) (cdr x2) null)))) (let ((_let_10 (ite ((_ is node) x3) (children x3) null))) (let ((_let_11 (cons (node (cons x3 x2)) null))) (let ((_let_12 (ite _let_1 (car null) _let_4))) (let ((_let_13 (cons _let_12 (ite ((_ is cons) _let_11) (cdr _let_11) null)))) (let ((_let_14 (ite ((_ is leaf) _let_12) (data _let_12) zero))) (let ((_let_15 (node x2))) (let ((_let_16 (ite ((_ is node) _let_15) (children _let_15) null))) (let ((_let_17 (ite ((_ is cons) _let_16) (car _let_16) _let_4))) (let ((_let_18 (node _let_2))) (let ((_let_19 (leaf (ite ((_ is leaf) _let_18) (data _let_18) zero)))) (let ((_let_20 (ite ((_ is node) _let_19) (children _let_19) null))) (let ((_let_21 (cons _let_15 (ite ((_ is node) _let_18) (children _let_18) null)))) (let ((_let_22 (ite ((_ is cons) _let_21) (cdr _let_21) null))) (let ((_let_23 (node (cons x3 (cons (ite ((_ is cons) _let_22) (car _let_22) _let_4) (cons (node (cons (node _let_16) (cons (ite ((_ is cons) _let_20) (car _let_20) _let_4) (ite ((_ is node) _let_17) (children _let_17) null)))) (cons (leaf (succ (ite ((_ is succ) _let_14) (pred _let_14) zero))) null))))))) (let ((_let_24 (ite ((_ is leaf) _let_23) (data _let_23) zero))) (not (and (and (and (and (and (and (and (not ((_ is succ) (ite ((_ is succ) _let_24) (pred _let_24) zero))) (= (node (ite ((_ is cons) _let_13) (cdr _let_13) null)) (ite ((_ is cons) _let_10) (car _let_10) _let_4))) ((_ is null) (ite ((_ is cons) _let_9) (cdr _let_9) null))) ((_ is null) (cons (leaf (ite ((_ is succ) _let_8) (pred _let_8) zero)) (ite ((_ is node) _let_6) (children _let_6) null)))) ((_ is node) _let_4)) (not (= x2 null))) (not ((_ is zero) (ite ((_ is leaf) _let_3) (data _let_3) zero)))) ((_ is null) (ite ((_ is cons) _let_2) (cdr _let_2) null))))))))))))))))))))))))))))) ))
