(include "animations.scm")
(include "math.scm")

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

(define delta-timer (ENGINE-timer-create))

(define logic-loop
  (lambda ()
    (cond
     [(not (ENGINE-is-running)) '()]
     [else
      (logic-step (ENGINE-timer-reset delta-timer))
      (logic-loop)])))

(define logic-step
  (lambda (dt)
    ;; TODO!
    '()))
