; EXPECT: sat
(set-logic ALL)
(set-option :incremental false)
(declare-datatypes ((RightistTree 1)) ((par (T)((node (left Bool) (right (RightistTree T))) (leaf (data T))))))
(declare-fun x () (RightistTree Int))
(declare-fun y () (RightistTree Real))
(assert (not (= x (leaf 0))))
(assert (> (data y) 1.0))
(assert (> (data x) 1))
(check-sat)
