; EXPECT: unsat
;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-datatypes ((St 0) (Ex 0) (List!2293 0))
(((Block!2236 (body!2237 List!2293)) (For!2238 (init!2239 St) (expr!2240 Ex) (step!2241 St) (body!2242 St)) (IfTE (expr!2244 Ex) (then!2245 St) (elze!2246 St)) (Skip!2250) (While (expr!2252 Ex) (body St)))
((Var!2291 (varID!2292 (_ BitVec 32))))
((Cons!2294 (head!2295 St) (tail!2296 List!2293)) (Nil!2297))
))
(declare-fun error_value!2298 () Bool)
(declare-fun error_value!2299 () List!2293)
(declare-fun s () St)
(declare-fun body!2242_uf_1 (St) St)
(declare-fun step!2241_uf_2 (St) St)
(declare-fun init!2239_uf_3 (St) St)
(declare-fun elze!2246_uf_4 (St) St)
(declare-fun then!2245_uf_5 (St) St)
(declare-fun body!2237_uf_6 (St) List!2293)
(declare-fun tail!2296_uf_7 (List!2293) List!2293)
(declare-fun head!2295_uf_8 (List!2293) St)
(declare-fun expr!2240_uf_9 (St) Ex)
(declare-fun body_uf_10 (St) St)
(declare-fun expr!2252_uf_11 (St) Ex)
(declare-fun expr!2244_uf_12 (St) Ex)
(declare-fun iwf (St) Bool)
(declare-fun iwfl (List!2293) Bool)
(declare-fun ewl (St) St)
(declare-fun ewlList!211 (List!2293) List!2293)
(declare-sort I_iwf 0)
(declare-fun iwf_arg_0_13 (I_iwf) St)
(declare-sort I_iwfl 0)
(declare-fun iwfl_arg_0_14 (I_iwfl) List!2293)
(declare-sort I_ewl 0)
(declare-fun ewl_arg_0_15 (I_ewl) St)
(declare-sort I_ewlList!211 0)
(declare-fun ewlList!211_arg_0_16 (I_ewlList!211) List!2293)
(declare-fun termITE_17 () St)
(declare-fun termITE_18 () St)
(declare-fun termITE_19 () St)
(declare-fun termITE_20 () St)

(assert
(and
(forall ((?i1 I_ewl)) (= (ewl (ewl_arg_0_15 ?i1))

(ite ((_ is IfTE) (ewl_arg_0_15 ?i1)) (IfTE (ite ((_ is IfTE) (ewl_arg_0_15 ?i1)) (expr!2244 (ewl_arg_0_15 ?i1)) (expr!2244_uf_12 (ewl_arg_0_15 ?i1))) (ewl (ite ((_ is IfTE) (ewl_arg_0_15 ?i1)) (then!2245 (ewl_arg_0_15 ?i1)) (then!2245_uf_5 (ewl_arg_0_15 ?i1)))) (ewl (ite ((_ is IfTE) (ewl_arg_0_15 ?i1)) (elze!2246 (ewl_arg_0_15 ?i1)) (elze!2246_uf_4 (ewl_arg_0_15 ?i1)))))

(ite ((_ is While) (ewl_arg_0_15 ?i1)) (For!2238 Skip!2250 (ite ((_ is While) (ewl_arg_0_15 ?i1)) (expr!2252 (ewl_arg_0_15 ?i1)) (expr!2252_uf_11 (ewl_arg_0_15 ?i1))) Skip!2250 (ewl (ite ((_ is While) (ewl_arg_0_15 ?i1)) (body (ewl_arg_0_15 ?i1)) (body_uf_10 (ewl_arg_0_15 ?i1)))))

(ite ((_ is For!2238) (ewl_arg_0_15 ?i1)) (For!2238 (ewl (ite ((_ is For!2238) (ewl_arg_0_15 ?i1)) (init!2239 (ewl_arg_0_15 ?i1)) (init!2239_uf_3 (ewl_arg_0_15 ?i1)))) (ite ((_ is For!2238) (ewl_arg_0_15 ?i1)) (expr!2240 (ewl_arg_0_15 ?i1)) (expr!2240_uf_9 (ewl_arg_0_15 ?i1))) (ewl (ite ((_ is For!2238) (ewl_arg_0_15 ?i1)) (step!2241 (ewl_arg_0_15 ?i1)) (step!2241_uf_2 (ewl_arg_0_15 ?i1)))) (ewl (ite ((_ is For!2238) (ewl_arg_0_15 ?i1)) (body!2242 (ewl_arg_0_15 ?i1)) (body!2242_uf_1 (ewl_arg_0_15 ?i1)))))

(ewl_arg_0_15 ?i1))))) )


(forall ((?i2 I_ewl)) (ite ((_ is IfTE) (ewl_arg_0_15 ?i2)) (and (not (forall ((?z I_ewl)) (not (= (ewl_arg_0_15 ?z) (ite ((_ is IfTE) (ewl_arg_0_15 ?i2)) (then!2245 (ewl_arg_0_15 ?i2)) (then!2245_uf_5 (ewl_arg_0_15 ?i2))))) )) (not (forall ((?z I_ewl)) (not (= (ewl_arg_0_15 ?z) (ite ((_ is IfTE) (ewl_arg_0_15 ?i2)) (elze!2246 (ewl_arg_0_15 ?i2)) (elze!2246_uf_4 (ewl_arg_0_15 ?i2))))) ))) (ite ((_ is While) (ewl_arg_0_15 ?i2)) (not (forall ((?z I_ewl)) (not (= (ewl_arg_0_15 ?z) (ite ((_ is While) (ewl_arg_0_15 ?i2)) (body (ewl_arg_0_15 ?i2)) (body_uf_10 (ewl_arg_0_15 ?i2))))) )) (ite ((_ is For!2238) (ewl_arg_0_15 ?i2)) (and (not (forall ((?z I_ewl)) (not (= (ewl_arg_0_15 ?z) (ite ((_ is For!2238) (ewl_arg_0_15 ?i2)) (init!2239 (ewl_arg_0_15 ?i2)) (init!2239_uf_3 (ewl_arg_0_15 ?i2))))) )) (not (forall ((?z I_ewl)) (not (= (ewl_arg_0_15 ?z) (ite ((_ is For!2238) (ewl_arg_0_15 ?i2)) (step!2241 (ewl_arg_0_15 ?i2)) (step!2241_uf_2 (ewl_arg_0_15 ?i2))))) )) (not (forall ((?z I_ewl)) (not (= (ewl_arg_0_15 ?z) (ite ((_ is For!2238) (ewl_arg_0_15 ?i2)) (body!2242 (ewl_arg_0_15 ?i2)) (body!2242_uf_1 (ewl_arg_0_15 ?i2))))) ))) true))) )
(forall ((?i3 I_iwf)) (= (iwf (iwf_arg_0_13 ?i3)) (ite ((_ is Block!2236) (iwf_arg_0_13 ?i3)) (iwfl (ite ((_ is Block!2236) (iwf_arg_0_13 ?i3)) (body!2237 (iwf_arg_0_13 ?i3)) (body!2237_uf_6 (iwf_arg_0_13 ?i3)))) (ite ((_ is IfTE) (iwf_arg_0_13 ?i3)) (and (iwf (ite ((_ is IfTE) (iwf_arg_0_13 ?i3)) (elze!2246 (iwf_arg_0_13 ?i3)) (elze!2246_uf_4 (iwf_arg_0_13 ?i3)))) (iwf (ite ((_ is IfTE) (iwf_arg_0_13 ?i3)) (then!2245 (iwf_arg_0_13 ?i3)) (then!2245_uf_5 (iwf_arg_0_13 ?i3))))) (ite ((_ is While) (iwf_arg_0_13 ?i3)) false (ite ((_ is For!2238) (iwf_arg_0_13 ?i3)) (and (iwf (ite ((_ is For!2238) (iwf_arg_0_13 ?i3)) (body!2242 (iwf_arg_0_13 ?i3)) (body!2242_uf_1 (iwf_arg_0_13 ?i3)))) (iwf (ite ((_ is For!2238) (iwf_arg_0_13 ?i3)) (step!2241 (iwf_arg_0_13 ?i3)) (step!2241_uf_2 (iwf_arg_0_13 ?i3)))) (iwf (ite ((_ is For!2238) (iwf_arg_0_13 ?i3)) (init!2239 (iwf_arg_0_13 ?i3)) (init!2239_uf_3 (iwf_arg_0_13 ?i3))))) true))))) )
(forall ((?i4 I_iwf)) (ite ((_ is Block!2236) (iwf_arg_0_13 ?i4)) (not (forall ((?z I_iwfl)) (not (= (iwfl_arg_0_14 ?z) (ite ((_ is Block!2236) (iwf_arg_0_13 ?i4)) (body!2237 (iwf_arg_0_13 ?i4)) (body!2237_uf_6 (iwf_arg_0_13 ?i4))))) )) (ite ((_ is IfTE) (iwf_arg_0_13 ?i4)) (and (not (forall ((?z I_iwf)) (not (= (iwf_arg_0_13 ?z) (ite ((_ is IfTE) (iwf_arg_0_13 ?i4)) (elze!2246 (iwf_arg_0_13 ?i4)) (elze!2246_uf_4 (iwf_arg_0_13 ?i4))))) )) (not (forall ((?z I_iwf)) (not (= (iwf_arg_0_13 ?z) (ite ((_ is IfTE) (iwf_arg_0_13 ?i4)) (then!2245 (iwf_arg_0_13 ?i4)) (then!2245_uf_5 (iwf_arg_0_13 ?i4))))) ))) (ite ((_ is While) (iwf_arg_0_13 ?i4)) true (ite ((_ is For!2238) (iwf_arg_0_13 ?i4)) (and (not (forall ((?z I_iwf)) (not (= (iwf_arg_0_13 ?z) (ite ((_ is For!2238) (iwf_arg_0_13 ?i4)) (body!2242 (iwf_arg_0_13 ?i4)) (body!2242_uf_1 (iwf_arg_0_13 ?i4))))) )) (not (forall ((?z I_iwf)) (not (= (iwf_arg_0_13 ?z) (ite ((_ is For!2238) (iwf_arg_0_13 ?i4)) (step!2241 (iwf_arg_0_13 ?i4)) (step!2241_uf_2 (iwf_arg_0_13 ?i4))))) )) (not (forall ((?z I_iwf)) (not (= (iwf_arg_0_13 ?z) (ite ((_ is For!2238) (iwf_arg_0_13 ?i4)) (init!2239 (iwf_arg_0_13 ?i4)) (init!2239_uf_3 (iwf_arg_0_13 ?i4))))) ))) true)))) )
((_ is IfTE) s)
(iwf s)
(not (forall ((?z I_iwf)) (not (= (iwf_arg_0_13 ?z) s)) ))
(ite ((_ is IfTE) s) (= termITE_17 (then!2245 s)) (= termITE_17 (then!2245_uf_5 s)))
(ite ((_ is IfTE) s) (= termITE_18 (then!2245 s)) (= termITE_18 (then!2245_uf_5 s)))
(=> (and (iwf termITE_17) (not (forall ((?z I_iwf)) (not (= (iwf_arg_0_13 ?z) termITE_18)) ))) (and (= (ewl termITE_17) termITE_17) (not (forall ((?z I_ewl)) (not (= (ewl_arg_0_15 ?z) termITE_18)) ))))
(ite ((_ is IfTE) s) (= termITE_19 (elze!2246 s)) (= termITE_19 (elze!2246_uf_4 s)))
(ite ((_ is IfTE) s) (= termITE_20 (elze!2246 s)) (= termITE_20 (elze!2246_uf_4 s)))
(=> (and (iwf termITE_19) (not (forall ((?z I_iwf)) (not (= (iwf_arg_0_13 ?z) termITE_20)) ))) (and (= (ewl termITE_19) termITE_19) (not (forall ((?z I_ewl)) (not (= (ewl_arg_0_15 ?z) termITE_20)) ))))
(not (= (ewl s) s))
(not (forall ((?z I_ewl)) (not (= (ewl_arg_0_15 ?z) s)) ))


)
)

(check-sat)
