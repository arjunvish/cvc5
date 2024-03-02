; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-logic ALL)
(declare-datatypes ((unitb 0)) (((ub (data (_ BitVec 1))))))

(declare-fun x () (Relation (_ BitVec 1) unitb (_ BitVec 1)))
(declare-fun y () (Relation (_ BitVec 1) unitb (_ BitVec 1)))
(declare-fun a () (_ BitVec 1))
(declare-fun b () (_ BitVec 1))
(declare-fun c () (_ BitVec 1))
(declare-fun d () (_ BitVec 1))
(declare-fun e () (_ BitVec 1))
(declare-fun u () unitb)
(assert (not (= b c)))
(assert (set.member (tuple a u b) x))
(assert (set.member (tuple a u c) x))
(assert (set.member (tuple d u a) y))
(assert (not (set.member (tuple a u u a) (rel.join x y))))
(check-sat)
