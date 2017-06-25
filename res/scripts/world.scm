;;;
;;; world.scm
;;;
;;; Cellular automata modelling theory applied to create interesting game maps.
;;;

(define clone-mat2d
  (lambda (mat2d)
    (define dim (array-dimensions mat2d))
    (define new-mat2d (make-array 0 (car dim) (car (cdr dim))))
    new-mat2d))

(define is-alive?
  (lambda (world x y)
    (if (array-in-bounds? world x y)
        (eq? (array-ref world x y) 1) #f)))

(define sum-neighbors
  (lambda (world x y)
    (define x-range (cons (- x 1) (+ x 1)))
    (define y-range (cons (- y 1) (+ y 1)))
    (define self-bias (array-ref world x y))
    (define do-sum
      (lambda (x-c y-c sum)
        (cond
         ((>= (cdr y-range) y-c)
          (cond
           ((>= (cdr x-range) x-c)
            (do-sum x-c (+ y-c 1) (if (is-alive? world x-c y-c) (+ sum 1) sum)))
           (else (- sum self-bias))))
         (else (do-sum (+ x-c 1) (car y-range) sum)))))
    (do-sum (car x-range) (car y-range) 0)))

(define die-off-thresholds (cons 5 2))

(define condense
  (lambda (world iters)
    (define new-world (clone-mat2d world))
    (cond
     ((eq? 0 iters) world)
     (else
      (array-index-map!
       new-world (lambda (x y)
                   (define count (sum-neighbors world x y))
                   (cond
                    ((eq? (array-ref world x y))
                     (if (>= count (car die-off-thresholds)) 0 1))
                    (else
                     (if (<= count (cdr die-off-thresholds)) 1 0)))))
      (condense new-world (- iters 1))))))

(define flood-fill
  (lambda (mat2d sub-color start-x start-y)
    (define new-mat2d (clone-mat2d mat2d))
    (define target-color (array-ref new-mat2d start-x start-y))
    (define flood-fill-impl
      (lambda (x y count)
        (if (array-in-bounds? new-mat2d x y)
            (cond
             ((eq? target-color (array-ref new-mat2d x y))
              (array-set! new-mat2d sub-color x y)
              (flood-fill-impl (+ x 1) y       (+ count 1))
              (flood-fill-impl (- x 1) y       (+ count 1))
              (flood-fill-impl x       (+ y 1) (+ count 1))
              (flood-fill-impl x       (- y 1) (+ count 1)))
             (else count))
            count)))
    (array-copy! mat2d new-mat2d)
    (cons new-mat2d (flood-fill-impl start-x start-y 0))))

(define auto-mask-region
  (lambda (mat2d)
    (define result (flood-fill mat2d 5 0 0))
    result))

(define create-world
  (lambda (width height margin)
    (define world (make-array 0 width height))
    (define mask-result '())
    (array-index-map! world (lambda (x y) (random 2)))
    (set! world (condense world 3))
    (set! mask-result (auto-mask-region world))
    mask-result))
