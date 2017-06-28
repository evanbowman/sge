;;;
;;; main.scm
;;;
;;; Entry point for the game, the engine starts executing
;;; scripts by looking up and calling the symbol 'main'.
;;;

(define entities '())


(include "math.scm")
(include "world-gen.scm")
(include "player.scm")

(define *game-state* (list 'startup))

(define (logic-step dt)
  (set! *game-state*
    (case (car *game-state*)
      ((startup)
       ;; TODO...
       (cons 'credits-screen 0))

      ((credits-screen)
       (let ((counter (cdr *game-state*)))
         (cond
          ((< counter 100000)
           (display "credits entry")
           (cons 'credits-screen (+ counter dt)))
          ((and (>= counter 100000) (<= counter 200000))
           (display "credits")
           (cons 'credits-screen (+ counter dt)))
          ((and (> counter 200000) (< counter 300000))
           (display "credits exit")
           (cons 'credits-screen (+ counter dt)))
          (else
           (list 'menu-level)))))
      
      ((menu-level)
       (for-each (lambda (entity)
                   (update-entity entity)) entities)
       (list 'menu-level)))))

(define (main)
  (logic-loop))

(define *delta-timer* (timer-create))

(define (logic-loop)
  (cond
   ((not (eng-is-running?)) '())
   (else
    (logic-step (timer-reset *delta-timer*))
    (logic-loop))))


;;;; TEST ;;;;

(define laika-run-anim
  (animation-create
   "game-objects.png"
   391 253 36 32 18 16))

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

