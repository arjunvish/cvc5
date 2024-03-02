; DISABLE-TESTER: lfsc
; DISABLE-TESTER: alf
; COMMAND-LINE: --strings-exp --seq-array=eager
; EXPECT: unsat
;; Logic not supported in Alethe
; DISABLE-TESTER: alethe
(set-logic ALL)
(declare-sort T 0)
(declare-sort T@ 0)
(declare-sort |T@[$| 0)
(declare-sort |T@[| 0)
(declare-datatypes ((@ 0)) ((($1 (c (Seq (Seq Int)))))))
(declare-datatypes ((@$ 0)) ((($1 (p @)))))
(declare-datatypes ((M 0)) (((M (o |T@[|)))))
(declare-datatypes ((T@$ 0)) (((T))))
(declare-datatypes ((@M 0)) (((M (|#| |T@[$|)))))
(declare-datatypes ((E 0)) (((E (s T@)))))
(declare-datatypes ((L 0)) (((a))))
(declare-datatypes (($M 0)) ((($M (l L) (p Int) (|#| @)))))
(declare-datatypes ((@$M 0)) (((M (l L) (p Int) (v (Seq (Seq Int)))))))
(declare-fun S (T@ T) @M)
(declare-fun S (|T@[$| T@$) Int)
(declare-fun S (|T@[| Int) @$)
(declare-fun e () E)
(declare-fun |1| () M)
(declare-fun _$ () (Seq Int))
(declare-fun $ () $M)
(declare-fun i () @$M)
(assert (not (=> (= (|#| $) (p (S (o |1|) 1))) (= i (M (l i) (p i) (seq.unit _$))) (= $ ($M (l $) (p $) ($1 (v i)))) (and (forall ((h T)) (and (forall ((v T@$)) (= 0 (S (|#| (S (s e) h)) T)))))) (= _$ (seq.nth (c (p (S (o |1|) 1))) (- 1 (seq.len (c (p (S (o |1|) 1))))))))))
(check-sat)
