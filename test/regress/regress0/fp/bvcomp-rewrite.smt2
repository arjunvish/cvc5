(set-logic QF_FP)
(declare-const x Float64)
(assert (fp.isNaN (fp.sub roundNearestTiesToEven (fp.mul roundNearestTiesToEven x (fp (_ bv0 1) (_ bv2047 11) (_ bv1 52))) (fp.mul roundNearestTiesToEven x x))))
(set-info :status sat)
(check-sat)