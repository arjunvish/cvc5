; DISABLE-TESTER: alf
;; Datatypes are not supported in Alethe
; DISABLE-TESTER: alethe
(set-option :global-declarations true)
(set-logic ALL)
(set-info :status unsat)
(declare-datatype _x0 ( (_x6 (_x1 Bool))))
(declare-datatype _x36 ( (_x42 (_x37 Bool))))
(declare-const _x53 Bool)
(declare-const _x56 _x36)
(declare-const _x58 _x0)
(declare-datatype D ( par ( A ) ( (c (s A)) (b (_x180 _x0)))) )
(assert (let ((_let0 ((_ update s) ((as b (D Int)) _x58) _x58)))(distinct _let0 _let0)))
(check-sat)
