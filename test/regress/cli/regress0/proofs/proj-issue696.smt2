; EXPECT: unsat
;; unsupported real.pi operator
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-option :proof-mode sat-proof)
(set-option :produce-unsat-assumptions true)
(set-option :incremental true)
(assert (<= real.pi (arctan real.pi)))
(check-sat-assuming (false))
