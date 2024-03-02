; EXPECT: unsat
;; Datatypes are not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-option :incremental true)
(declare-datatypes ((DT1 0)) (((DT1_a) (DT1_b) (DT1_c) (DT1_d) (DT1_e) (DT1_f) (DT1_g) (DT1_h) (DT1_i) (DT1_j) (DT1_k) (DT1_l) (DT1_m) (DT1_n) (DT1_o) (DT1_p) (DT1_q) (DT1_r) (DT1_s) (DT1_t) (DT1_u) (DT1_v) (DT1_w) (DT1_x) (DT1_y) (DT1_z))))
(declare-datatypes ((DT2 0)) (((DT2_a) (DT2_b) (DT2_c) (DT2_d))))
(declare-datatypes ((DT3 0)) (((DT3_a) (DT3_b))))
(declare-fun var1 () DT3)
(declare-fun var2 () DT3)
(declare-fun var3 () DT1)
(declare-fun var4 () DT3)
(declare-fun var5 () DT3)
(declare-fun var6 () DT3)
(declare-fun var7 () DT3)
(declare-fun var8 () DT3)
(declare-fun var9 () DT3)
(declare-fun var10 () DT3)
(declare-fun var11 () DT2)
(declare-fun var12 () DT3)
(declare-fun var13 () DT3)
(declare-fun var14 () DT3)
(declare-fun var16 () DT3)
(declare-fun var17 () DT3)
(declare-fun var18 () DT3)
(declare-fun var20 () DT3)
(declare-fun var21 () DT3)
(push 1)

(assert (let ((_let_1 (or (or (and (and (= var7 DT3_b) (= var4 DT3_b)) (= var1 DT3_a)) (= (or (and (= var5 DT3_a) (= var17 DT3_b)) (and (= var21 DT3_b) (or (= var3 DT1_f) (= var3 DT1_g)))) (= DT3_b DT3_b))) (and (and (= var14 DT3_a) (= var2 DT3_a)) (or (or (or (and (= var8 DT3_a) (= var18 DT3_b)) (and (= var6 DT3_a) (not (= var11 DT2_a)))) (= var20 DT3_b)) (= var9 DT3_b)))))) (and (and (and (not (= var13 DT3_a)) (not (= var10 DT3_b))) (not (or _let_1 (and (not _let_1) (and (= var14 DT3_b) true))))) (not (or (= var12 DT3_a) (and (= var12 DT3_b) (or (= var16 DT3_b) true)))))))

(check-sat)

(pop 1)
