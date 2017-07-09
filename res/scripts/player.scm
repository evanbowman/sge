;;;
;;; player.scm
;;;
;;; Player update logic script.
;;;

(define player-run-cycle
  (animation-create
   "player-run-cycle.png"
   0 0 21 44 10 20))

(class Player
  ((engine-handle (entity-create))
   (anim-timer 0)
   (keyframe 0))

  ((init)
   (lambda ()
     (entity-set-animation engine-handle player-run-cycle)
     (entity-set-position engine-handle 100 100)
     (entity-set-scale engine-handle 2.0 2.0)))
  
  ((update)
   (lambda (dt)
     (cond
      ((> anim-timer 60000)
       (set! anim-timer 0)
       (set! keyframe (if (< keyframe 10) (+ keyframe 1) 4))
       (entity-set-keyframe engine-handle keyframe))
      (else (set! anim-timer (+ anim-timer dt))))))
  
  ((remove)
   (lambda ()
     (entity-remove engine-handle))))
