;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(set-option :check-proofs true)
(set-option :proof-check eager)
(declare-const x Real)
(assert (= real.pi (/ real.pi (tan (to_real (to_int x))))))
(assert (= 1 (to_int x)))
(check-sat)
