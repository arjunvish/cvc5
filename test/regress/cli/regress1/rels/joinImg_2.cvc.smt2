; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-logic ALL)
(set-option :sets-ext true)
(declare-sort Atom 0)
(declare-fun x () (Relation Atom Atom))
(declare-fun y () (Relation Atom Atom))
(declare-fun r () (Relation Atom Atom))
(declare-fun t () (Relation Atom))
(declare-fun a () Atom)
(declare-fun b () Atom)
(declare-fun c () Atom)
(declare-fun d () Atom)
(declare-fun e () Atom)
(declare-fun f () Atom)
(declare-fun g () Atom)
(assert (set.member (tuple a) (rel.join_image x 2)))
(assert (set.member (tuple a) (rel.join_image y 3)))
(assert (= x (set.union (set.union (set.union (set.union (set.singleton (tuple f g)) (set.singleton (tuple b c))) (set.singleton (tuple d e))) (set.singleton (tuple c e))) (set.singleton (tuple f b)))))
(assert (set.member (tuple a f) x))
(assert (set.member (tuple a f) y))
(assert (= x y))
(assert (not (= a b)))
(assert (not (set.member (tuple d) (rel.join_image x 2))))
(assert (= f d))
(check-sat)
