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

(define *delta-timer* (timer-create))

(define (main)
  (logic-loop))

(define (logic-loop)
  (cond ((not (eng-is-running?)) '())
   (else
    (logic-step (timer-reset *delta-timer*))
    (logic-loop))))

(set! *current-level* (Level))
(*current-level* 'enter)

(define player (Player))
(player 'init)

(define (logic-step dt)
  (player 'update dt)
  (*current-level* 'update dt))
