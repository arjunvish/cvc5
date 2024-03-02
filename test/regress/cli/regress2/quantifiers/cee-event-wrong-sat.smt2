; COMMAND-LINE: -q --full-saturate-quant --ee-mode=distributed
; COMMAND-LINE: -q --full-saturate-quant --ee-mode=central
; EXPECT: unsat
; DISABLE-TESTER: unsat-core
;; introduces fresh Skolem in a trusted step
; DISABLE-TESTER: alethe
(set-logic ALL)
(set-info :status unsat)
(declare-datatypes ((T@$TypeValue 0) (T@A 0)) (((A)) ((T))))
(declare-datatypes ((T@$Location 0)) (((P) (G (|t# | T@$TypeValue) (|a#G| Int)))))
(declare-sort |T@[Int]$Value| 0)
(declare-datatypes ((T@$Value 0) (T@R 0)) (((E) ($Boolean (|b#$Boolean| Bool)) (I (|i#I| Int)) (D (|a#D| Int)) (V (|v#V| T@R))) ((R (|v#R| |T@[Int]$Value|) (|l#R| Int)))))
(declare-sort b 0)
(declare-sort l 0)
(declare-datatypes ((T@M 0)) (((M (|domain#M| b) (|contents#M| l)))))
(declare-fun $EmptyValueArray () T@R)
(declare-fun m (T@$Value) |T@[Int]$Value|)
(declare-fun $IsEqual_stratified (T@$Value T@$Value) Bool)
(declare-fun sel (|T@[Int]$Value| Int) T@$Value)
(declare-fun st (|T@[Int]$Value| Int T@$Value) |T@[Int]$Value|)
(assert (forall ((?x0 |T@[Int]$Value|) (?x1 Int) (?x2 T@$Value)) (= ?x2 (sel (st ?x0 ?x1 ?x2) ?x1))))
(assert (forall ((?x0 |T@[Int]$Value|) (?x1 Int) (?y1 Int) (?x2 T@$Value)) (= (sel ?x0 ?y1) (sel (st ?x0 ?x1 ?x2) ?y1))))
(declare-fun $LibraAccount_T_type_value () T@$TypeValue)
(declare-fun s (l T@$Location) T@$Value)
(declare-fun $Event_EventHandleGenerator_counter () Int)
(declare-fun $Event_EventHandleGenerator_type_value () T@$TypeValue)
(assert (= 0 (|l#R| $EmptyValueArray)))
(assert (forall ((v1 T@$Value) (v2 T@$Value)) (= ($IsEqual_stratified v1 v2) (or (= v1 v2) (forall ((i Int)) (or (> 0 i) (>= i (|l#R| (|v#V| v1))) ($IsEqual_stratified (sel (|v#R| (|v#V| v1)) i) (sel (|v#R| (|v#V| v2)) i))))))))
(assert (forall ((m@@0 T@M) (a@@0 T@$Value)) (! (or (not (is-D a@@0)) (and (is-I (sel (|v#R| (|v#V| (s (|contents#M| m@@0) (G $Event_EventHandleGenerator_type_value (|a#D| a@@0))))) $Event_EventHandleGenerator_counter)) (forall ((x@@10 Int)) (or (> x@@10 0) (= E (sel (|v#R| (|v#V| (s (|contents#M| m@@0) (G $LibraAccount_T_type_value (|a#D| a@@0))))) x@@10)))))) :qid || :skolemid ||)))
(declare-fun |Select_[$Location]$bool| (b T@$Location) Bool)
(declare-fun $m@@0 () T@M)
(declare-fun $abort_flag@2 () Bool)
(declare-fun $m@3 () T@M)
(declare-fun mv () T@$TypeValue)
(declare-fun inline$$MoveToRaw$0$a@0 () Int)
(declare-fun |Store_[$Location]$bool| (b T@$Location Bool) b)
(assert (forall ((?x0 b) (?x1 T@$Location) (?y1 T@$Location) (?x2 Bool)) (or (= ?x1 ?y1) (= (|Select_[$Location]$bool| ?x0 ?y1) (|Select_[$Location]$bool| (|Store_[$Location]$bool| ?x0 ?x1 ?x2) ?y1)))))
(declare-fun inline$$MoveToRaw$0$l@1 () T@$Location)
(declare-fun |Store_[$Location]$Value| (l T@$Location T@$Value) l)
(assert (forall ((?x0 l) (?x1 T@$Location) (?x2 T@$Value)) (= ?x2 (s (|Store_[$Location]$Value| ?x0 ?x1 ?x2) ?x1))))
(assert (forall ((?x0 l) (?x1 T@$Location) (?y1 T@$Location) (?x2 T@$Value)) (= (s ?x0 ?y1) (s (|Store_[$Location]$Value| ?x0 ?x1 ?x2) ?y1))))
(declare-fun inline$$Event_EventHandleGenerator_pack$0$$struct@1 () T@$Value)
(declare-fun inline$$Event_publish_generator$0$$tmp@1 () T@$Value)
(declare-fun i () T@$Value)
(assert (and (not $abort_flag@2) (= inline$$Event_publish_generator$0$$tmp@1 (I 0)) (|b#$Boolean| ($Boolean (forall ((addr@@9 T@$Value)) (or (is-D addr@@9) (|b#$Boolean| ($Boolean (|b#$Boolean| ($Boolean (|Select_[$Location]$bool| (|domain#M| $m@@0) (G mv (|a#D| addr@@9))))))))))) (or (not (=> $abort_flag@2 (|b#$Boolean| ($Boolean (forall ((addr@@4 T@$Value)) (|b#$Boolean| ($Boolean (|b#$Boolean| ($Boolean (|b#$Boolean| ($Boolean (|b#$Boolean| ($Boolean (|Select_[$Location]$bool| (|domain#M| $m@@0) (G mv (|a#D| addr@@4)))))))))))))))) (and (= inline$$Event_EventHandleGenerator_pack$0$$struct@1 (V (R (st (st (m E) 0 inline$$Event_publish_generator$0$$tmp@1) 1 i) 1))) (not (=> (= inline$$MoveToRaw$0$a@0 (|a#D| i)) (= inline$$MoveToRaw$0$l@1 (G mv inline$$MoveToRaw$0$a@0)) (or (|b#$Boolean| ($Boolean (|Select_[$Location]$bool| (|domain#M| $m@@0) (G mv (|a#D| i))))) (not (= $m@3 (M (|Store_[$Location]$bool| (|domain#M| $m@@0) inline$$MoveToRaw$0$l@1 true) (|Store_[$Location]$Value| (|contents#M| $m@@0) inline$$MoveToRaw$0$l@1 inline$$Event_EventHandleGenerator_pack$0$$struct@1)))) (and (|b#$Boolean| ($Boolean ($IsEqual_stratified (s (|contents#M| $m@@0) (G mv (|a#D| i))) (V (R (|v#R| (R (|v#R| $EmptyValueArray) 0)) 1))))) (or (not (|b#$Boolean| ($Boolean ($IsEqual_stratified (s (|contents#M| $m@@0) (G mv (|a#D| i))) (V (R (|v#R| (R (st (|v#R| $EmptyValueArray) (|l#R| $EmptyValueArray) (I 0)) 1)) 0)))))) (and (|b#$Boolean| ($Boolean (forall ((addr@@1 T@$Value)) (|b#$Boolean| ($Boolean (or (not (|b#$Boolean| ($Boolean (|Select_[$Location]$bool| (|domain#M| $m@@0) (G mv (|a#D| addr@@1)))))) (|b#$Boolean| ($Boolean (|b#$Boolean| ($Boolean ($IsEqual_stratified (sel (|v#R| (|v#V| (s (|contents#M| $m@3) (G mv (|a#D| addr@@1))))) $Event_EventHandleGenerator_counter) (sel (|v#R| (|v#V| (s (|contents#M| $m@@0) (G mv (|a#D| addr@@1))))) $Event_EventHandleGenerator_counter)))))))))))) (|b#$Boolean| ($Boolean (forall ((addr@@3 T@$Value)) (|b#$Boolean| ($Boolean (or (|b#$Boolean| ($Boolean ($IsEqual_stratified (I 0) (sel (|v#R| (|v#V| (s (|contents#M| $m@@0) (G $Event_EventHandleGenerator_type_value (|a#D| addr@@3))))) $Event_EventHandleGenerator_counter)))) (not (|b#$Boolean| ($Boolean (and (|b#$Boolean| ($Boolean (|Select_[$Location]$bool| (|domain#M| $m@3) (G mv (|a#D| addr@@3))))) (|b#$Boolean| ($Boolean (not (|b#$Boolean| ($Boolean (|Select_[$Location]$bool| (|domain#M| $m@@0) (G $LibraAccount_T_type_value (|a#D| addr@@3))))))))))))))))))))))))))))
(check-sat)
