;;;
;;; main.scm
;;;
;;; Entry point for the game, the engine starts executing
;;; scripts by looking up and calling the symbol 'main'.
;;;

(include "class.scm")
(include "math.scm")
(include "player.scm")
(include "level.scm")
(include "wall.scm")

(define (main)
  (logic-loop))

(define *delta-timer* (timer-create))
(define *logic-timer* (timer-create))
(define *low-power-mode* #t)

(define (logic-loop)
  (cond ((not (eng-is-running?)) '())
   (else
    (logic-step (timer-reset *delta-timer*))
    (cond
     (*low-power-mode*
      (let ((logic-usec (timer-reset *logic-timer*)))
        (micro-sleep (max 0 (- 2000 logic-usec))))))
    (logic-loop))))

(define *player* (Player))
(*player* 'init)

(camera-set-target (*player* 'get-handle))
(camera-set-springiness 1.5)
(camera-set-zoom 2)

(switch-level apartment-0)

(define (logic-step dt)
  (*current-level* 'update dt))
