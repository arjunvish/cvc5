;; introduces RE Skolem
; DISABLE-TESTER: alethe
(set-logic QF_S)
(set-info :status unsat)
(declare-const X String)
(assert (str.in_re X (re.++ (re.range "1" "9") ((_ re.loop 0 2) (re.range "0" "9")) (str.to_re "}"))))
(assert (not (str.in_re X (re.++ (re.union (re.range "0" "9") (re.++ (re.range "1" "9") (re.range "0" "9")) (re.++ (re.range "1" "9") (re.range "0" "9") (re.range "0" "9"))) (str.to_re "}")))))
(check-sat)
