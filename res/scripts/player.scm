;;;
;;; player.scm
;;;
;;; Player update logic script.
;;;
;;; The Player logic is basically a big state machine. Should
;;; be pretty easy to follow how it works.
;;;

(define player-run-cycle
  (animation-create
   "player-run-cycle.png"
   0 0 21 44 10 20))

(define (sgn num)
  (if (> num 0) 1.0 -1.0))

(class Player
  ((entity-handle (entity-create))
   (anim-timer 0)
   (keyframe 0)
   (state 'idle)
   (x-pos 0)
   (y-pos 0)
   (x-speed 0.0)
   (y-speed 0.0)
   (health 5)
   (x-accel-counter 0))

  ((init)
   (lambda ()
     (entity-set-animation entity-handle player-run-cycle)
     (entity-set-position entity-handle 0 0)
     (entity-set-scale entity-handle 2.0 2.0)))

  ((get-handle)
   (lambda () entity-handle))
  
  ((reset-with-position)
   (lambda (x y)
     (set! x-pos x)
     (set! y-pos y)
     (set! x-speed 0)
     (set! y-speed 0)
     (set! health 5)
     (set! x-accel-counter 0)
     (set! state 'idle)
     (set! keyframe 0)
     (entity-set-keyframe entity-handle 0)))
  
  ((update)
   (lambda (dt)
     (define (run-impl stop-key)
       (cond ((not (key-pressed? stop-key))
              (set! state 'idle)
              (set! keyframe 0)
              (entity-set-keyframe entity-handle 0)))
       (cond
        ((> anim-timer 60000)
         (set! anim-timer 0)
         (set! keyframe (if (< keyframe 10) (+ keyframe 1) 4))
         (entity-set-keyframe entity-handle keyframe))
        (else (set! anim-timer (+ anim-timer dt))))
       (set! x-speed (lerp (* 0.6 (sgn x-speed)) x-speed (* dt 0.0000025))))
     (set! x-accel-counter (+ x-accel-counter dt))
     
     (case state
       ((run-left)
        (run-impl key-left))

       ((run-right)
        (run-impl key-right))
       
       ((idle)
        (cond
         ((key-pressed? key-left)
          (set! state 'run-left)
          (set! x-speed -0.01)
          (entity-set-scale entity-handle -2.0 2.0))
         ((key-pressed? key-right)
          (set! state 'run-right)
          (set! x-speed 0.01)
          (entity-set-scale entity-handle 2.0 2.0)))
        (set! x-speed (lerp 0.000 x-speed (* dt 0.000015))))
       
       ((jumping)
        '()))
     (let ((rate-factor (* dt 0.001)))
       (set! x-pos (+ x-pos (* x-speed rate-factor)))
       (set! y-pos (+ y-pos (* y-speed rate-factor)))
       (entity-set-position entity-handle x-pos y-pos))))
  
  ((remove)
   (lambda ()
     (entity-remove entity-handle))))
