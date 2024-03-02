;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-info :smt-lib-version 2.6)
(set-logic ALL)
(set-info :status unsat)
(set-option :strings-exp true)
(set-option :re-elim agg)
(declare-const x String)
(declare-const y String)
(assert (str.in_re x (re.* (str.to_re "'\r''k'\n'"))))
(assert (str.in_re x (re.* (str.to_re "'\r''k'\n''\r''k'\n'"))))
(assert (> (str.len x) 20))
(assert (< (str.len x) 25))
(check-sat)
