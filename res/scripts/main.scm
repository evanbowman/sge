(include "animations.scm")
(include "math.scm")
(include "world.scm")
(include "astar.scm")

(define (main)
  (catch #t
    (lambda ()
      (logic-loop))
    (lambda (key . parameters)
      (format
       (current-error-port)
       "Uncaught throw to '~a: ~a\n"
       key parameters))))

(define delta-timer (timer-create))

(define (logic-loop)
  (cond
   ((not (eng-is-running?)) '())
   (else
    (logic-step (timer-reset delta-timer))
    (logic-loop))))

(define entities '())

(define (init-entities count)
  (define new-entity '())
  (cond ((< count 30)
         (set! new-entity (entity-create))
         (entity-set-animation new-entity laika-run-anim)
         (entity-set-position new-entity 400 (* count 30))
         (set! entities (cons new-entity entities)) (init-entities (+ count 1)))
        (else '())))

(init-entities 0)

(define (update-entity entity)
  (define pos (entity-get-position entity))
  (define kf (entity-get-keyframe entity))
  (micro-sleep 1000)
  (entity-set-position entity (- (car pos) 5) (cdr pos))
  (entity-set-keyframe entity (cond
                               ((> kf 3) 0)
                               (else (+ kf 1)))))

(define (logic-step dt)
  (define (update-entities entities)
    (cond
     ((not (null? entities))
      (update-entity (car entities))
      (update-entities (cdr entities)))
     (else '())))
  (update-entities entities)
  ;; (define e1-pos (entity-get-position e1))
  ;; (define e1-kf (entity-get-keyframe e1))
  ;; (micro-sleep 50000)
  ;; (entity-set-position e1 (- (car e1-pos) 5) (cdr e1-pos))
  ;; (entity-set-keyframe e1 (cond
  ;;                          ((> e1-kf 3) 0)
  ;;                          (else (+ e1-kf 1))))
  '())
