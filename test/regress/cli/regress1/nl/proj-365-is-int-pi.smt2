;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(set-option :nl-ext light)
(assert (is_int real.pi))
(check-sat)
