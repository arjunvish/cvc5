;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-const x String)
(declare-const x8 (Seq String))
(assert (ite (seq.prefixof (seq.unit x) x8) false true))
(assert (seq.suffixof (seq.unit true) (seq.unit (seq.prefixof (seq.unit x8) (seq.unit (seq.unit x))))))
(check-sat)
