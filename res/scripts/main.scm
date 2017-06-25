(include "animations.scm")
(include "math.scm")
(include "world.scm")

(define delta-timer (eng-timer-create))

(set! *random-state* (random-state-from-platform))

(define main
  (lambda ()
    (catch #t
      (lambda ()
        (logic-loop))
      (lambda (key . parameters)
        (format
         (current-error-port)
         "Uncaught throw to '~a: ~a\n"
         key parameters)))))

(define logic-loop
  (lambda ()
    (cond
     [(not (eng-is-running?)) '()]
     [else
      (logic-step (eng-timer-reset delta-timer))
      (logic-loop)])))

(define logic-step
  (lambda (dt)
    '()))
