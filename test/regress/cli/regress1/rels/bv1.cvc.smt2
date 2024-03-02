; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-logic ALL)

(declare-fun x () (Relation (_ BitVec 1) (_ BitVec 1)))
(declare-fun y () (Relation (_ BitVec 1) (_ BitVec 1)))
(declare-fun a () (_ BitVec 1))
(declare-fun b () (_ BitVec 1))
(declare-fun c () (_ BitVec 1))
(declare-fun d () (_ BitVec 1))
(declare-fun e () (_ BitVec 1))
(assert (not (= b c)))
(assert (set.member (tuple a b) x))
(assert (set.member (tuple a c) x))
(assert (set.member (tuple d a) y))
(assert (not (set.member (tuple a a) (rel.join x y))))
(check-sat)
