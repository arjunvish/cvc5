; COMMAND-LINE: --no-produce-proofs
(set-logic NIA)
(set-info :status sat)
(set-option :repeat-simp true)
(set-option :on-repeat-ite-simp true)
(set-option :ite-simp true)
(declare-const i0 Int)
(assert (or (= i0 0) (= i0 0)))
(check-sat)
