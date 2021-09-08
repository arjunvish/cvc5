; COMMAND-LINE: --arith-eq-solver --ee-mode=distributed
; COMMAND-LINE: --arith-eq-solver --ee-mode=central
; EXPECT: unsat
(set-logic ALL)
(declare-fun v () String)
(assert (str.in_re (str.++ "" v) (re.* (str.to_re "z"))))
(assert (str.in_re v (re.* (re.range "a" "u"))))
(assert (not (str.in_re v (str.to_re ""))))
(check-sat)
