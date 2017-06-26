;;;
;;; math.scm
;;;
;;; Commonly useful math functions for game programming.
;;;

(define (clamp x floor ceil)
  (cond
   ((< x floor) floor)
   ((> x ceil) ceil)
   (else x)))

(define (lerp a b t)
  (+ (* a t) (* b (- 1.0 t))))

(define (smoothstep a b t)
  (define x (clamp (/ (- x a) (- b a)) 0.f 1.f))
  (* x (* x (- 3 (* 2 x)))))

(define (smootherstep a b t)
  (define x (clamp (/ (- x a) (- b a)) 0.f 1.f))
  (* x (* x (* x (+ 10 (* x (- (* x 6) 15)))))))

(define (distance x1 y1 x2 y2)
  (sqrt (+ (expt (- x2 x1) 2) (expt (- y2 y1) 2))))
