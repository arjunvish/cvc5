; EXPECT: unsat
;; Datatypes are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-option :incremental false)
(declare-datatypes ((nat 0)(list 0)(tree 0)) (((succ (pred nat)) (zero))((cons (car tree) (cdr list)) (null))((node (children list)) (leaf (data nat)))))
(declare-fun x1 () nat)
(declare-fun x2 () nat)
(declare-fun x3 () nat)
(declare-fun x4 () nat)
(declare-fun x5 () nat)
(declare-fun x6 () nat)
(declare-fun x7 () nat)
(declare-fun x8 () nat)
(declare-fun x9 () nat)
(declare-fun x10 () nat)
(declare-fun x11 () list)
(declare-fun x12 () list)
(declare-fun x13 () list)
(declare-fun x14 () list)
(declare-fun x15 () list)
(declare-fun x16 () list)
(declare-fun x17 () list)
(declare-fun x18 () list)
(declare-fun x19 () list)
(declare-fun x20 () list)
(declare-fun x21 () tree)
(declare-fun x22 () tree)
(declare-fun x23 () tree)
(declare-fun x24 () tree)
(declare-fun x25 () tree)
(declare-fun x26 () tree)
(declare-fun x27 () tree)
(declare-fun x28 () tree)
(declare-fun x29 () tree)
(declare-fun x30 () tree)
(check-sat-assuming ( (not (not (and (and (and (not ((_ is zero) x3)) (= x8 zero)) (not (= x25 x28))) (not ((_ is zero) x8))))) ))
