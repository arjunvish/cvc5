;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-fun A () (Set Int))
(declare-fun B () (Set Int))
(declare-fun C () (Set Int))

(declare-fun f () Int)
(declare-fun g () Int)
(declare-fun h () Int)
(declare-fun i () Int)
(declare-fun j () Int)

(assert (set.subset A (set.insert g h i (set.singleton f))))
(assert (= C (set.minus A B) ))
(assert (set.subset B A))
(assert (= C (set.inter A B)))
(assert (set.member j C))
(check-sat)
