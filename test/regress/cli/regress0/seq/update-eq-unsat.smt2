; COMMAND-LINE: --strings-exp --seq-array=eager
; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(declare-fun x () (Seq Int))

(assert (= (str.len x) 1))
(assert (= (seq.update x 0 (seq.unit 1)) (seq.update x 0 (seq.unit 2))))

(check-sat)
