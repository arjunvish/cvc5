;; FP not supported in Alethe
; DISABLE-TESTER: alethe
(set-info :smt-lib-version 2.6)
(set-logic FPLRA)
(set-info :status unsat)
(declare-fun c_main_~E0~7 () (_ FloatingPoint 11 53))
(declare-fun c_main_~S~7 () (_ FloatingPoint 11 53))
(assert (and (= ((_ to_fp 11 53) RNE (_ bv0 32)) c_main_~S~7) (fp.geq c_main_~E0~7 (fp.neg ((_ to_fp 11 53) RNE 1.0))) (fp.leq c_main_~E0~7 ((_ to_fp 11 53) RNE 1.0))))
(assert (not (and (exists ((main_~E0~7 (_ FloatingPoint 11 53)) (main_~E1~7 (_ FloatingPoint 11 53))) (and (fp.geq main_~E1~7 (fp.neg ((_ to_fp 11 53) RNE 1.0))) (= c_main_~S~7 (fp.sub RNE (fp.add RNE (fp.mul RNE ((_ to_fp 11 53) RNE 0.999) ((_ to_fp 11 53) RNE (_ bv0 32))) main_~E0~7) main_~E1~7)) (fp.geq main_~E0~7 (fp.neg ((_ to_fp 11 53) RNE 1.0))) (fp.leq main_~E0~7 ((_ to_fp 11 53) RNE 1.0)) (fp.leq main_~E1~7 ((_ to_fp 11 53) RNE 1.0)))) (fp.geq c_main_~E0~7 (fp.neg ((_ to_fp 11 53) RNE 1.0))) (fp.leq c_main_~E0~7 ((_ to_fp 11 53) RNE 1.0)))))
(check-sat)
(exit)
