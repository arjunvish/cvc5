; EXPECT: unsat
;; Datatypes are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(declare-datatype List (par (T) ((nil) (cons (head T) (tail (List T))))))

(declare-const x (List Int))
(assert (not (exists ((y (List Int))) (= x (tail y)))))
(check-sat)
