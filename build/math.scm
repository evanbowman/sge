
(define clamp
  (lambda (x floor ceil)
    ;; Clamps a value so that it's bounded by floor and ceil.
    (cond
     [(< x floor) floor]
     [(> x ceil) ceil]
     [else x])))

(define lerp
  (lambda (a b t)
    ;; Linearly interpolate a value between a and b based on fraction t.
    (+ (* a t) (* b (- 1.0 t)))))

(define smoothstep
  (lambda (a b t)
    ;; Like lerp, but interpolates a hermite curve for a smoother result.
    (define x (clamp (/ (- x a) (- b a)) 0.f 1.f))
    (* x (* x (- 3 (* 2 x))))))

(define smootherstep
  (lambda (a b t)
    ;; Like smoothstep, but samples an even smoother curve.
    (define x (clamp (/ (- x a) (- b a)) 0.f 1.f))
    (* x (* x (* x (+ 10 (* x (- (* x 6) 15))))))))

(define distance
  (lambda (x1 y1 x2 y2)
    ;; The classic distance function, between two cartesian coordinates.
    (sqrt (+ (expt (- x2 x1) 2) (expt (- y2 y1) 2)))))

