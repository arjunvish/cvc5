; DISABLE-TESTER: lfsc
; COMMAND-LINE: --bv-solver=bitblast
;; introduces arrays Skolem
; DISABLE-TESTER: alethe
(set-option :incremental false)
(set-info :status unsat)
(set-logic QF_AUFBV)
(declare-fun start2 () (_ BitVec 32))
(declare-fun start1 () (_ BitVec 32))
(declare-fun a1 () (Array (_ BitVec 32) (_ BitVec 8)))
(check-sat-assuming ( (let ((_let_0 (bvand (bvnot (bvand (bvnot (select (store (store a1 start1 (_ bv0 8)) (bvadd (_ bv3 32) start1) (_ bv0 8)) (_ bv0 32))) (bvnot (select (store (store a1 start1 (_ bv0 8)) (bvadd (_ bv3 32) start1) (_ bv0 8)) (_ bv0 32))))) (bvnot (bvand (select (store (store a1 start1 (_ bv0 8)) (bvadd (_ bv3 32) start1) (_ bv0 8)) (_ bv0 32)) (select (store (store a1 start1 (_ bv0 8)) (bvadd (_ bv3 32) start1) (_ bv0 8)) (_ bv0 32))))))) (let ((_let_1 (bvand (bvnot (bvand (bvnot (select (store (store a1 start1 (_ bv0 8)) (bvadd (_ bv3 32) start1) (_ bv0 8)) (_ bv0 32))) (bvnot _let_0))) (bvnot (bvand (select (store (store a1 start1 (_ bv0 8)) (bvadd (_ bv3 32) start1) (_ bv0 8)) (_ bv0 32)) _let_0))))) (let ((_let_2 (store (store (store (store a1 start1 (_ bv0 8)) (bvadd (_ bv3 32) start1) (_ bv0 8)) (_ bv0 32) _let_1) (_ bv0 32) (bvand (bvnot (bvand (bvnot _let_0) (bvnot _let_1))) (bvnot (bvand _let_0 _let_1)))))) (let ((_let_3 (bvadd (_ bv3 32) start2))) (let ((_let_4 (select _let_2 _let_3))) (let ((_let_5 (select _let_2 start2))) (let ((_let_6 (bvand (bvnot (bvand (bvnot _let_4) (bvnot _let_5))) (bvnot (bvand _let_4 _let_5))))) (let ((_let_7 (bvand (bvnot (bvand (bvnot _let_4) (bvnot _let_6))) (bvnot (bvand _let_4 _let_6))))) (let ((_let_8 (store (store _let_2 _let_3 _let_7) start2 (bvand (bvnot (bvand (bvnot _let_6) (bvnot _let_7))) (bvnot (bvand _let_6 _let_7)))))) (let ((_let_9 (select _let_8 (_ bv0 32)))) (let ((_let_10 (select _let_8 start2))) (let ((_let_11 (bvand (bvnot (bvand (bvnot _let_9) (bvnot _let_10))) (bvnot (bvand _let_9 _let_10))))) (let ((_let_12 (bvand (bvnot (bvand (bvnot _let_9) (bvnot _let_11))) (bvnot (bvand _let_9 _let_11))))) (let ((_let_13 (bvand (bvnot (bvand (bvnot (select (store (store _let_8 (_ bv0 32) _let_12) start2 (bvand (bvnot (bvand (bvnot _let_11) (bvnot _let_12))) (bvnot (bvand _let_11 _let_12)))) (_ bv0 32))) (bvnot (select (store (store _let_8 (_ bv0 32) _let_12) start2 (bvand (bvnot (bvand (bvnot _let_11) (bvnot _let_12))) (bvnot (bvand _let_11 _let_12)))) (_ bv0 32))))) (bvnot (bvand (select (store (store _let_8 (_ bv0 32) _let_12) start2 (bvand (bvnot (bvand (bvnot _let_11) (bvnot _let_12))) (bvnot (bvand _let_11 _let_12)))) (_ bv0 32)) (select (store (store _let_8 (_ bv0 32) _let_12) start2 (bvand (bvnot (bvand (bvnot _let_11) (bvnot _let_12))) (bvnot (bvand _let_11 _let_12)))) (_ bv0 32))))))) (let ((_let_14 (bvand (bvnot (bvand (bvnot (select (store (store _let_8 (_ bv0 32) _let_12) start2 (bvand (bvnot (bvand (bvnot _let_11) (bvnot _let_12))) (bvnot (bvand _let_11 _let_12)))) (_ bv0 32))) (bvnot _let_13))) (bvnot (bvand (select (store (store _let_8 (_ bv0 32) _let_12) start2 (bvand (bvnot (bvand (bvnot _let_11) (bvnot _let_12))) (bvnot (bvand _let_11 _let_12)))) (_ bv0 32)) _let_13))))) (let ((_let_15 (select (store (store a1 (bvadd (_ bv3 32) start1) (_ bv0 8)) start1 (_ bv0 8)) (_ bv0 32)))) (let ((_let_16 (store (store (store (store a1 (bvadd (_ bv3 32) start1) (_ bv0 8)) start1 (_ bv0 8)) (_ bv0 32) _let_15) (_ bv0 32) _let_15))) (let ((_let_17 (store (store _let_16 _let_3 (select _let_16 start2)) start2 (select _let_16 _let_3)))) (let ((_let_18 (select (store (store _let_17 (_ bv0 32) (select _let_17 start2)) start2 (select _let_17 (_ bv0 32))) (_ bv0 32)))) (not (= (_ bv0 1) (bvnot (ite (= (store (store (store (store _let_8 (_ bv0 32) _let_12) start2 (bvand (bvnot (bvand (bvnot _let_11) (bvnot _let_12))) (bvnot (bvand _let_11 _let_12)))) (_ bv0 32) _let_14) (_ bv0 32) (bvand (bvnot (bvand (bvnot _let_13) (bvnot _let_14))) (bvnot (bvand _let_13 _let_14)))) (store (store (store (store _let_17 (_ bv0 32) (select _let_17 start2)) start2 (select _let_17 (_ bv0 32))) (_ bv0 32) _let_18) (_ bv0 32) _let_18)) (_ bv1 1) (_ bv0 1)))))))))))))))))))))))) ))
