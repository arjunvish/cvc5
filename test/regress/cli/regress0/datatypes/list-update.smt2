;; Datatypes are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-datatypes ((list 0)) (
((cons (head Int) (tail list)) (nil))
))
(declare-fun a () list)
(declare-fun b () list)
(assert ((_ is cons) a))
(assert (= ((_ update head) a 3) ((_ update head) b 4)))
(check-sat)
