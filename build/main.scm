
(define main
  (lambda ()
    (logic-loop)))

(define delta-timer (ENGINE-create-timer))

(define logic-loop
  (lambda ()
    (cond
     [(not (ENGINE-is-running)) '()]
     [else
      (logic-step (ENGINE-reset-timer delta-timer))
      (logic-loop)])))

(define logic-step
  (lambda (dt)
    (display dt)
    (display "\n")))
