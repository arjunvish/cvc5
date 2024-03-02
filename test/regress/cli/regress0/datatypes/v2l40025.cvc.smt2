; EXPECT: unsat
;; Datatypes are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-option :incremental false)
(declare-datatypes ((nat 0)(list 0)(tree 0)) (((succ (pred nat)) (zero))((cons (car tree) (cdr list)) (null))((node (children list)) (leaf (data nat)))))
(declare-fun x1 () nat)
(declare-fun x2 () nat)
(declare-fun x3 () list)
(declare-fun x4 () list)
(declare-fun x5 () tree)
(declare-fun x6 () tree)
(check-sat-assuming ( (not (not (and (and (and ((_ is leaf) x5) (not (= x2 x1))) (= x2 (pred (pred zero)))) (not ((_ is node) (node null)))))) ))
