;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic QF_ALL)
(set-info :status unsat)
(set-option :check-proofs true)
(set-option :proof-check eager)
(assert (> real.pi (seq.nth (seq.++ (seq.++ (seq.unit real.pi) (seq.rev (seq.++ (seq.unit real.pi) (seq.unit real.pi)))) (seq.unit real.pi) (seq.unit real.pi)) (seq.len (seq.++ (seq.unit real.pi) (seq.unit real.pi))))))
(check-sat)