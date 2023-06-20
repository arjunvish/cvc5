; COMMAND-LINE: --ho-elim
(set-logic HO_ALL)
(set-info :status unsat)
(declare-sort $$unsorted 0)
(declare-sort tptp.int 0)
(declare-sort tptp.nat 0)
(declare-fun tptp.one_one_int () tptp.int)
(declare-fun tptp.one_one_nat () tptp.nat)
(declare-fun tptp.plus_plus_int (tptp.int tptp.int) tptp.int)
(declare-fun tptp.plus_plus_nat (tptp.nat tptp.nat) tptp.nat)
(declare-fun tptp.zero_zero_int () tptp.int)
(declare-fun tptp.zero_zero_nat () tptp.nat)
(declare-fun tptp.bit0 (tptp.int) tptp.int)
(declare-fun tptp.bit1 (tptp.int) tptp.int)
(declare-fun tptp.pls () tptp.int)
(declare-fun tptp.number_number_of_int (tptp.int) tptp.int)
(declare-fun tptp.number_number_of_nat (tptp.int) tptp.nat)
(declare-fun tptp.semiri1621563631at_int (tptp.nat) tptp.int)
(declare-fun tptp.semiri984289939at_nat (tptp.nat) tptp.nat)
(declare-fun tptp.ord_less_int (tptp.int tptp.int) Bool)
(declare-fun tptp.ord_less_nat (tptp.nat tptp.nat) Bool)
(declare-fun tptp.power_power_int (tptp.int tptp.nat) tptp.int)
(declare-fun tptp.power_power_nat (tptp.nat tptp.nat) tptp.nat)
(declare-fun tptp.n () tptp.nat)
(declare-fun tptp.t () tptp.int)
(assert (@ (@ tptp.ord_less_int tptp.zero_zero_int) (@ (@ tptp.plus_plus_int tptp.one_one_int) (@ tptp.semiri1621563631at_int tptp.n))))
(assert (@ (@ tptp.ord_less_int tptp.one_one_int) tptp.t))
(assert (forall ((X_3 tptp.int) (Y_3 tptp.int)) (let ((_let_1 (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls))))) (= (= (@ (@ tptp.plus_plus_int (@ (@ tptp.power_power_int X_3) _let_1)) (@ (@ tptp.power_power_int Y_3) _let_1)) tptp.zero_zero_int) (and (= X_3 tptp.zero_zero_int) (= Y_3 tptp.zero_zero_int))))))
(assert (= (@ (@ tptp.power_power_int tptp.one_one_int) (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls)))) tptp.one_one_int))
(assert (= (@ (@ tptp.power_power_nat tptp.one_one_nat) (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls)))) tptp.one_one_nat))
(assert (= (@ (@ tptp.power_power_int tptp.zero_zero_int) (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls)))) tptp.zero_zero_int))
(assert (= (@ (@ tptp.power_power_nat tptp.zero_zero_nat) (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls)))) tptp.zero_zero_nat))
(assert (forall ((A_7 tptp.int)) (= (= (@ (@ tptp.power_power_int A_7) (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls)))) tptp.zero_zero_int) (= A_7 tptp.zero_zero_int))))
(assert (forall ((W_7 tptp.int)) (= (@ (@ tptp.plus_plus_int tptp.one_one_int) (@ tptp.number_number_of_int W_7)) (@ tptp.number_number_of_int (@ (@ tptp.plus_plus_int (@ tptp.bit1 tptp.pls)) W_7)))))
(assert (forall ((V_5 tptp.int)) (= (@ (@ tptp.plus_plus_int (@ tptp.number_number_of_int V_5)) tptp.one_one_int) (@ tptp.number_number_of_int (@ (@ tptp.plus_plus_int V_5) (@ tptp.bit1 tptp.pls))))))
(assert (= (@ (@ tptp.plus_plus_int tptp.one_one_int) tptp.one_one_int) (@ tptp.number_number_of_int (@ tptp.bit0 (@ tptp.bit1 tptp.pls)))))
(assert (= (@ (@ tptp.plus_plus_int tptp.one_one_int) tptp.one_one_int) (@ tptp.number_number_of_int (@ tptp.bit0 (@ tptp.bit1 tptp.pls)))))
(assert (= (@ (@ tptp.plus_plus_nat tptp.one_one_nat) tptp.one_one_nat) (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls)))))
(assert (forall ((X_3 tptp.int)) (let ((_let_1 (@ tptp.bit0 (@ tptp.bit1 tptp.pls)))) (let ((_let_2 (@ tptp.power_power_int X_3))) (let ((_let_3 (@ tptp.number_number_of_nat _let_1))) (= (@ (@ tptp.power_power_int (@ _let_2 _let_3)) _let_3) (@ _let_2 (@ tptp.number_number_of_nat (@ tptp.bit0 _let_1)))))))))
(assert (forall ((W_6 tptp.int)) (let ((_let_1 (@ tptp.number_number_of_nat W_6))) (let ((_let_2 (@ (@ tptp.power_power_int tptp.zero_zero_int) _let_1))) (let ((_let_3 (= _let_1 tptp.zero_zero_nat))) (and (=> _let_3 (= _let_2 tptp.one_one_int)) (=> (not _let_3) (= _let_2 tptp.zero_zero_int))))))))
(assert (forall ((W_6 tptp.int)) (let ((_let_1 (@ tptp.number_number_of_nat W_6))) (let ((_let_2 (@ (@ tptp.power_power_nat tptp.zero_zero_nat) _let_1))) (let ((_let_3 (= _let_1 tptp.zero_zero_nat))) (and (=> _let_3 (= _let_2 tptp.one_one_nat)) (=> (not _let_3) (= _let_2 tptp.zero_zero_nat))))))))
(assert (= tptp.one_one_int (@ tptp.number_number_of_int (@ tptp.bit1 tptp.pls))))
(assert (= (@ tptp.number_number_of_int (@ tptp.bit1 tptp.pls)) tptp.one_one_int))
(assert (@ (@ tptp.ord_less_nat tptp.zero_zero_nat) tptp.n))
(assert (forall ((X_3 tptp.int) (Y_3 tptp.int)) (or (@ (@ tptp.ord_less_int X_3) Y_3) (= X_3 Y_3) (@ (@ tptp.ord_less_int Y_3) X_3))))
(assert (forall ((K tptp.int) (L tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.number_number_of_int K)) (@ tptp.number_number_of_int L)) (@ (@ tptp.ord_less_int K) L))))
(assert (forall ((V_3 tptp.int) (W_4 tptp.int)) (= (@ (@ tptp.plus_plus_int (@ tptp.number_number_of_int V_3)) (@ tptp.number_number_of_int W_4)) (@ tptp.number_number_of_int (@ (@ tptp.plus_plus_int V_3) W_4)))))
(assert (forall ((X_6 tptp.int) (Y_5 tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.number_number_of_int X_6)) (@ tptp.number_number_of_int Y_5)) (@ (@ tptp.ord_less_int X_6) Y_5))))
(assert (= tptp.zero_zero_int (@ tptp.number_number_of_int tptp.pls)))
(assert (forall ((M tptp.nat) (N_1 tptp.nat)) (= (@ (@ tptp.power_power_int (@ tptp.semiri1621563631at_int M)) N_1) (@ tptp.semiri1621563631at_int (@ (@ tptp.power_power_nat M) N_1)))))
(assert (forall ((M tptp.nat) (N_1 tptp.nat)) (= (@ tptp.semiri1621563631at_int (@ (@ tptp.power_power_nat M) N_1)) (@ (@ tptp.power_power_int (@ tptp.semiri1621563631at_int M)) N_1))))
(assert (forall ((M tptp.nat) (N_1 tptp.nat) (Z tptp.int)) (= (@ (@ tptp.plus_plus_int (@ tptp.semiri1621563631at_int M)) (@ (@ tptp.plus_plus_int (@ tptp.semiri1621563631at_int N_1)) Z)) (@ (@ tptp.plus_plus_int (@ tptp.semiri1621563631at_int (@ (@ tptp.plus_plus_nat M) N_1))) Z))))
(assert (forall ((M tptp.nat) (N_1 tptp.nat)) (= (@ (@ tptp.plus_plus_int (@ tptp.semiri1621563631at_int M)) (@ tptp.semiri1621563631at_int N_1)) (@ tptp.semiri1621563631at_int (@ (@ tptp.plus_plus_nat M) N_1)))))
(assert (= (@ tptp.semiri1621563631at_int tptp.one_one_nat) tptp.one_one_int))
(assert (= (@ tptp.number_number_of_nat tptp.pls) tptp.zero_zero_nat))
(assert (= tptp.zero_zero_nat (@ tptp.number_number_of_nat tptp.pls)))
(assert (forall ((N_1 tptp.nat)) (= (= (@ tptp.semiri1621563631at_int N_1) tptp.zero_zero_int) (= N_1 tptp.zero_zero_nat))))
(assert (= (@ tptp.semiri1621563631at_int tptp.zero_zero_nat) tptp.zero_zero_int))
(assert (= (@ (@ tptp.plus_plus_nat tptp.one_one_nat) tptp.one_one_nat) (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls)))))
(assert (forall ((K1 tptp.int) (K2 tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.bit1 K1)) (@ tptp.bit1 K2)) (@ (@ tptp.ord_less_int K1) K2))))
(assert (forall ((K tptp.int) (L tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.bit1 K)) (@ tptp.bit1 L)) (@ (@ tptp.ord_less_int K) L))))
(assert (not (@ (@ tptp.ord_less_int tptp.pls) tptp.pls)))
(assert (forall ((K1 tptp.int) (K2 tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.bit0 K1)) (@ tptp.bit0 K2)) (@ (@ tptp.ord_less_int K1) K2))))
(assert (forall ((K tptp.int) (L tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.bit0 K)) (@ tptp.bit0 L)) (@ (@ tptp.ord_less_int K) L))))
(assert (forall ((K tptp.int) (I tptp.int) (J tptp.int)) (=> (@ (@ tptp.ord_less_int I) J) (@ (@ tptp.ord_less_int (@ (@ tptp.plus_plus_int I) K)) (@ (@ tptp.plus_plus_int J) K)))))
(assert (forall ((V_4 tptp.int) (V_3 tptp.int)) (let ((_let_1 (@ tptp.number_number_of_nat V_4))) (let ((_let_2 (@ tptp.number_number_of_nat V_3))) (let ((_let_3 (@ (@ tptp.plus_plus_nat _let_2) _let_1))) (let ((_let_4 (@ (@ tptp.ord_less_int V_4) tptp.pls))) (let ((_let_5 (@ (@ tptp.ord_less_int V_3) tptp.pls))) (and (=> _let_5 (= _let_3 _let_1)) (=> (not _let_5) (and (=> _let_4 (= _let_3 _let_2)) (=> (not _let_4) (= _let_3 (@ tptp.number_number_of_nat (@ (@ tptp.plus_plus_int V_3) V_4))))))))))))))
(assert (= tptp.one_one_int (@ tptp.number_number_of_int (@ tptp.bit1 tptp.pls))))
(assert (= (@ tptp.number_number_of_nat (@ tptp.bit1 tptp.pls)) tptp.one_one_nat))
(assert (= tptp.one_one_nat (@ tptp.number_number_of_nat (@ tptp.bit1 tptp.pls))))
(assert (forall ((X_5 tptp.int) (Y_4 tptp.int)) (= (= (@ tptp.number_number_of_int X_5) (@ tptp.number_number_of_int Y_4)) (= X_5 Y_4))))
(assert (forall ((W_5 tptp.int) (X_4 tptp.nat)) (let ((_let_1 (@ tptp.number_number_of_nat W_5))) (= (= _let_1 X_4) (= X_4 _let_1)))))
(assert (forall ((W_5 tptp.int) (X_4 tptp.int)) (let ((_let_1 (@ tptp.number_number_of_int W_5))) (= (= _let_1 X_4) (= X_4 _let_1)))))
(assert (forall ((K tptp.int) (L tptp.int)) (= (= (@ tptp.bit1 K) (@ tptp.bit1 L)) (= K L))))
(assert (forall ((K tptp.int) (L tptp.int)) (= (= (@ tptp.bit0 K) (@ tptp.bit0 L)) (= K L))))
(assert (forall ((A_6 tptp.int)) (= (@ (@ tptp.ord_less_int (@ (@ tptp.plus_plus_int A_6) A_6)) tptp.zero_zero_int) (@ (@ tptp.ord_less_int A_6) tptp.zero_zero_int))))
(assert (forall ((Z1 tptp.int) (Z2 tptp.int) (Z3 tptp.int)) (let ((_let_1 (@ tptp.plus_plus_int Z1))) (= (@ (@ tptp.plus_plus_int (@ _let_1 Z2)) Z3) (@ _let_1 (@ (@ tptp.plus_plus_int Z2) Z3))))))
(assert (forall ((X_3 tptp.int) (Y_3 tptp.int) (Z tptp.int)) (let ((_let_1 (@ tptp.plus_plus_int X_3))) (let ((_let_2 (@ tptp.plus_plus_int Y_3))) (= (@ _let_1 (@ _let_2 Z)) (@ _let_2 (@ _let_1 Z)))))))
(assert (forall ((Z tptp.int) (W_4 tptp.int)) (= (@ (@ tptp.plus_plus_int Z) W_4) (@ (@ tptp.plus_plus_int W_4) Z))))
(assert (forall ((M tptp.nat) (N_1 tptp.nat)) (= (= (@ tptp.semiri1621563631at_int M) (@ tptp.semiri1621563631at_int N_1)) (= M N_1))))
(assert (forall ((X_2 tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.number_number_of_int X_2)) tptp.zero_zero_int) (@ (@ tptp.ord_less_int X_2) tptp.pls))))
(assert (forall ((Y_2 tptp.int)) (= (@ (@ tptp.ord_less_int tptp.zero_zero_int) (@ tptp.number_number_of_int Y_2)) (@ (@ tptp.ord_less_int tptp.pls) Y_2))))
(assert (forall ((K tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.bit1 K)) tptp.pls) (@ (@ tptp.ord_less_int K) tptp.pls))))
(assert (forall ((K1 tptp.int) (K2 tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.bit1 K1)) (@ tptp.bit0 K2)) (@ (@ tptp.ord_less_int K1) K2))))
(assert (forall ((K tptp.int) (L tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.bit1 K)) (@ tptp.bit0 L)) (@ (@ tptp.ord_less_int K) L))))
(assert (forall ((K tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.bit0 K)) tptp.pls) (@ (@ tptp.ord_less_int K) tptp.pls))))
(assert (forall ((K tptp.int)) (let ((_let_1 (@ tptp.ord_less_int tptp.pls))) (= (@ _let_1 (@ tptp.bit0 K)) (@ _let_1 K)))))
(assert (forall ((W_4 tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.bit1 W_4)) tptp.zero_zero_int) (@ (@ tptp.ord_less_int W_4) tptp.zero_zero_int))))
(assert (not (@ (@ tptp.ord_less_int tptp.pls) tptp.zero_zero_int)))
(assert (forall ((W_4 tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.bit0 W_4)) tptp.zero_zero_int) (@ (@ tptp.ord_less_int W_4) tptp.zero_zero_int))))
(assert (@ (@ tptp.ord_less_int tptp.zero_zero_int) tptp.one_one_int))
(assert (forall ((W_4 tptp.int) (Z tptp.int)) (let ((_let_1 (@ tptp.ord_less_int W_4))) (= (@ _let_1 (@ (@ tptp.plus_plus_int Z) tptp.one_one_int)) (or (@ _let_1 Z) (= W_4 Z))))))
(assert (forall ((K tptp.nat)) (not (@ (@ tptp.ord_less_int (@ tptp.semiri1621563631at_int K)) tptp.zero_zero_int))))
(assert (forall ((X_1 tptp.int)) (= (@ (@ tptp.ord_less_int (@ tptp.number_number_of_int X_1)) tptp.one_one_int) (@ (@ tptp.ord_less_int X_1) (@ tptp.bit1 tptp.pls)))))
(assert (forall ((Y_1 tptp.int)) (= (@ (@ tptp.ord_less_int tptp.one_one_int) (@ tptp.number_number_of_int Y_1)) (@ (@ tptp.ord_less_int (@ tptp.bit1 tptp.pls)) Y_1))))
(assert (forall ((Z tptp.int)) (= (@ (@ tptp.ord_less_int (@ (@ tptp.plus_plus_int (@ (@ tptp.plus_plus_int tptp.one_one_int) Z)) Z)) tptp.zero_zero_int) (@ (@ tptp.ord_less_int Z) tptp.zero_zero_int))))
(assert (forall ((A_5 tptp.int)) (= (= (@ (@ tptp.plus_plus_int A_5) A_5) tptp.zero_zero_int) (= A_5 tptp.zero_zero_int))))
(assert (forall ((K tptp.int)) (not (= (@ tptp.bit1 K) tptp.pls))))
(assert (forall ((L tptp.int)) (not (= tptp.pls (@ tptp.bit1 L)))))
(assert (forall ((K tptp.int) (L tptp.int)) (not (= (@ tptp.bit1 K) (@ tptp.bit0 L)))))
(assert (forall ((K tptp.int) (L tptp.int)) (not (= (@ tptp.bit0 K) (@ tptp.bit1 L)))))
(assert (forall ((K tptp.int)) (= (= (@ tptp.bit0 K) tptp.pls) (= K tptp.pls))))
(assert (forall ((L tptp.int)) (= (= tptp.pls (@ tptp.bit0 L)) (= tptp.pls L))))
(assert (= (@ tptp.bit0 tptp.pls) tptp.pls))
(assert (= tptp.pls tptp.zero_zero_int))
(assert (not (= tptp.zero_zero_int tptp.one_one_int)))
(assert (forall ((K tptp.int)) (= (@ (@ tptp.plus_plus_int K) tptp.pls) K)))
(assert (forall ((K tptp.int)) (= (@ (@ tptp.plus_plus_int tptp.pls) K) K)))
(assert (forall ((K tptp.int) (L tptp.int)) (= (@ (@ tptp.plus_plus_int (@ tptp.bit0 K)) (@ tptp.bit0 L)) (@ tptp.bit0 (@ (@ tptp.plus_plus_int K) L)))))
(assert (forall ((K tptp.int)) (= (@ tptp.bit0 K) (@ (@ tptp.plus_plus_int K) K))))
(assert (forall ((Z tptp.int)) (= (@ (@ tptp.plus_plus_int Z) tptp.zero_zero_int) Z)))
(assert (forall ((Z tptp.int)) (= (@ (@ tptp.plus_plus_int tptp.zero_zero_int) Z) Z)))
(assert (= (@ tptp.number_number_of_int tptp.pls) tptp.zero_zero_int))
(assert (= (@ tptp.number_number_of_nat tptp.pls) tptp.zero_zero_nat))
(assert (= (@ tptp.number_number_of_int tptp.pls) tptp.zero_zero_int))
(assert (= tptp.zero_zero_int (@ tptp.number_number_of_int tptp.pls)))
(assert (forall ((A_4 tptp.int)) (= (@ (@ tptp.plus_plus_int (@ tptp.number_number_of_int tptp.pls)) A_4) A_4)))
(assert (forall ((A_3 tptp.int)) (= (@ (@ tptp.plus_plus_int A_3) (@ tptp.number_number_of_int tptp.pls)) A_3)))
(assert (forall ((A_2 tptp.int) (W_3 tptp.int)) (let ((_let_1 (@ tptp.number_number_of_nat W_3))) (= (= (@ (@ tptp.power_power_int A_2) _let_1) tptp.zero_zero_int) (and (= A_2 tptp.zero_zero_int) (not (= _let_1 tptp.zero_zero_nat)))))))
(assert (forall ((A_2 tptp.nat) (W_3 tptp.int)) (let ((_let_1 (@ tptp.number_number_of_nat W_3))) (= (= (@ (@ tptp.power_power_nat A_2) _let_1) tptp.zero_zero_nat) (and (= A_2 tptp.zero_zero_nat) (not (= _let_1 tptp.zero_zero_nat)))))))
(assert (forall ((V_2 tptp.int) (W_2 tptp.int) (Z_1 tptp.int)) (= (@ (@ tptp.plus_plus_int (@ tptp.number_number_of_int V_2)) (@ (@ tptp.plus_plus_int (@ tptp.number_number_of_int W_2)) Z_1)) (@ (@ tptp.plus_plus_int (@ tptp.number_number_of_int (@ (@ tptp.plus_plus_int V_2) W_2))) Z_1))))
(assert (forall ((V_1 tptp.int) (W_1 tptp.int)) (= (@ (@ tptp.plus_plus_int (@ tptp.number_number_of_int V_1)) (@ tptp.number_number_of_int W_1)) (@ tptp.number_number_of_int (@ (@ tptp.plus_plus_int V_1) W_1)))))
(assert (forall ((V tptp.int) (W tptp.int)) (= (@ tptp.number_number_of_int (@ (@ tptp.plus_plus_int V) W)) (@ (@ tptp.plus_plus_int (@ tptp.number_number_of_int V)) (@ tptp.number_number_of_int W)))))
(assert (forall ((K tptp.int) (L tptp.int)) (= (@ (@ tptp.plus_plus_int (@ tptp.bit1 K)) (@ tptp.bit0 L)) (@ tptp.bit1 (@ (@ tptp.plus_plus_int K) L)))))
(assert (forall ((K tptp.int) (L tptp.int)) (= (@ (@ tptp.plus_plus_int (@ tptp.bit0 K)) (@ tptp.bit1 L)) (@ tptp.bit1 (@ (@ tptp.plus_plus_int K) L)))))
(assert (forall ((K tptp.int)) (= (@ tptp.bit1 K) (@ (@ tptp.plus_plus_int (@ (@ tptp.plus_plus_int tptp.one_one_int) K)) K))))
(assert (forall ((Z tptp.int)) (not (= (@ (@ tptp.plus_plus_int (@ (@ tptp.plus_plus_int tptp.one_one_int) Z)) Z) tptp.zero_zero_int))))
(assert (forall ((N tptp.nat)) (= (@ tptp.number_number_of_nat (@ tptp.semiri1621563631at_int N)) (@ tptp.semiri984289939at_nat N))))
(assert (forall ((N tptp.nat)) (let ((_let_1 (@ tptp.semiri1621563631at_int N))) (= (@ tptp.number_number_of_int _let_1) _let_1))))
(assert (forall ((A_1 tptp.int)) (= (@ (@ tptp.ord_less_int tptp.zero_zero_int) (@ (@ tptp.power_power_int A_1) (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls))))) (not (= A_1 tptp.zero_zero_int)))))
(assert (forall ((A tptp.int)) (not (@ (@ tptp.ord_less_int (@ (@ tptp.power_power_int A) (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls))))) tptp.zero_zero_int))))
(assert (forall ((X tptp.int) (Y tptp.int)) (let ((_let_1 (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls))))) (= (@ (@ tptp.ord_less_int tptp.zero_zero_int) (@ (@ tptp.plus_plus_int (@ (@ tptp.power_power_int X) _let_1)) (@ (@ tptp.power_power_int Y) _let_1))) (or (not (= X tptp.zero_zero_int)) (not (= Y tptp.zero_zero_int)))))))
(assert (not (not (= (@ (@ tptp.power_power_int (@ (@ tptp.plus_plus_int tptp.one_one_int) (@ tptp.semiri1621563631at_int tptp.n))) (@ tptp.number_number_of_nat (@ tptp.bit0 (@ tptp.bit1 tptp.pls)))) tptp.zero_zero_int))))
(set-info :filename NUM925^1)
(check-sat-assuming ( true ))
