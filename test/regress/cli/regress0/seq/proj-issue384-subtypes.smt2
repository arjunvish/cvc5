;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(set-option :global-declarations true)
(set-option :strings-exp true)
(assert (let ((_let0 (seq.unit real.pi)))(seq.suffixof _let0 (seq.update _let0 (seq.len _let0) _let0))))
(assert (let ((_let0 real.pi))(let ((_let1 (- _let0)))(let ((_let2 (+ _let1 _let0)))(>= _let1 (* _let2 _let2) _let1)))))
(check-sat)
