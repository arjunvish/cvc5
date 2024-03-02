;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_ALL)
(set-info :status unsat)
(set-option :produce-models true)
(set-option :sets-ext true)
(declare-fun A () (Set (_ BitVec 2)))
(declare-fun B () (Set (_ BitVec 2)))
(declare-fun C () (Set (_ BitVec 2)))
(declare-fun D () (Set (_ BitVec 2)))

(declare-fun x () (_ BitVec 2))
(assert (not (set.member x A)))
(assert (not (set.member x B)))
(assert (not (set.member x C)))
(assert (not (set.member x D)))
(declare-fun y () (_ BitVec 2))
(assert (not (set.member y A)))
(assert (not (set.member y B)))
(assert (not (set.member y C)))
(assert (not (set.member y D)))
(declare-fun z () (_ BitVec 2))
(assert (not (set.member z A)))
(assert (not (set.member z B)))
(assert (not (set.member z C)))
(assert (not (set.member z D)))

(assert (distinct x y z))

(assert (= (set.card (set.union A (set.union B (set.union C D)))) 2))

(check-sat)
