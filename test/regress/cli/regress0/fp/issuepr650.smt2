(set-logic ALL)
(set-info :status sat)
(set-option :sets-ext true)
(set-option :fp-exp true)
(assert (set.subset (set.complement (set.singleton (_ -oo 5 11))) (set.singleton (fp.abs (set.choose (set.complement (set.singleton (_ -oo 5 11))))))))
(check-sat)
