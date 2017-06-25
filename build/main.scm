(include "animations.scm")

(define main
  (lambda ()
    (logic-loop)))

(define delta-timer (ENGINE-timer-create))

(define logic-loop
  (lambda ()
    (cond
     [(not (ENGINE-is-running)) '()]
     [else
      (logic-step (ENGINE-timer-reset delta-timer))
      (logic-loop)])))

(define times '())
(define num-times 0)

(define logic-step
  (lambda (dt)
    ;; TODO!
    '()))
