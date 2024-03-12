; EXPECT: unsat
;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-option :incremental false)
(set-option :fmf-fun true)
(define-funs-rec ((iseven ((x Int)) Int) (isodd ((y Int)) Int) (fact ((x Int)) Int)) ((ite (= x 0) 1 (ite (= (isodd (- x 1)) 0) 1 0)) (ite (= y 0) 0 (ite (= (iseven (- y 1)) 0) 1 0)) (ite (= x 0) 1 (* x (fact (- x 1))))))
(declare-fun a () Int)
(declare-fun b () Int)
(declare-fun x () Int)
(declare-fun y () Int)
(declare-fun z () Int)
(assert (= 1 (isodd 4)))
(assert (= 0 (iseven 4)))
(assert (= 0 (isodd 3)))
(assert (= 1 (iseven 3)))
(assert (not (= 24 (fact 4))))
(check-sat)
