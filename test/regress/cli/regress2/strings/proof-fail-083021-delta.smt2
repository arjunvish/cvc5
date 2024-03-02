;; introduces STOI Skolem
; DISABLE-TESTER: alethe
(set-logic QF_SLIA)
(set-info :status unsat)
(declare-const x Bool)
(declare-const x1 Int)
(declare-fun s () String)
(assert (ite (str.prefixof "-" (str.substr s 0 1)) (= 0 (str.to_int (str.substr (str.substr s 0 (+ 1 1)) 1 (- (str.len (str.substr s 0 (+ 1 1))) 1)))) true))
(assert (not (<= (- (str.len s) 1 (+ 1 1) 1) 3)))
(assert (ite x true (> 1 (str.len (str.substr s (+ 1 1 1 1) (- (str.len s) (+ 1 1 1 1 1)))))))
(assert (ite (str.prefixof "-" (str.substr s (+ 1 1) 1)) (= 0 (str.to_int (str.substr (str.substr s (+ 1 1) (+ 1 1 1)) 1 (- (str.len (str.substr s 1 x1)) 1)))) true))
(assert (str.in_re s (re.+ (re.range "0" "9"))))
(assert (not (<= (str.to_int (str.substr s 0 (+ 1 1))) 255)))
(check-sat)
