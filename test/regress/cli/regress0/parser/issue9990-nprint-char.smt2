; EXPECT: (error "Parse Error: issue9990-nprint-char.smt2:6.33: Non-printable character in string literal")
; EXIT: 1
; DISABLE-TESTER: dump
; REQUIRES: no-competition
(set-logic QF_S)
(assert (str.prefixof "/signin" "/signin "))
(check-sat)
