;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_ALL)
(set-info :status unsat)
(declare-heap (Int Int))

(declare-const x1 Int)
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const x4 Int)
(declare-const x5 Int)
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const x8 Int)
(declare-const x9 Int)

(declare-const a1 Int)
(declare-const a2 Int)
(declare-const a3 Int)
(declare-const a4 Int)
(declare-const a5 Int)
(declare-const a6 Int)
(declare-const a7 Int)
(declare-const a8 Int)
(declare-const a9 Int)

(assert (and (pto x1 a1) (pto x2 a2) (pto x3 a3)
         (pto x4 a4) (pto x5 a5) (pto x6 a6)
         (pto x7 a7) (pto x8 a8) (pto x9 a9)
    )
)

(assert (not (and (= x1 x2 x3 x4 x5 x6 x7 x8 x9)
          (= a1 a2 a3 a4 a5 a6 a7 a8 a9)
       )
    )
)

(check-sat)
