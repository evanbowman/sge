;;;
;;; class.scm
;;;
;;; A class in the game's code is really just a convenience
;;; macro for grouping functions with data. It doesn't support
;;; inheritance or dynamically adding methods at runtime or
;;; anything like that.
;;;
;;; Example usage:
;;;
;;; (class example
;;;   ((my-var 1))                       ; Member var list
;;;
;;;   ((incr) (lambda (x)                ; Method definition
;;;      (set! my-var (+ x my-var)))))
;;;
;;; (define ex (example))                ; instantiate class
;;; (ex 'incr 42)                        ; my-var is now 43
;;;

(define-syntax class
  (syntax-rules ()
    ((class name members methods ...)
     (define (name)
       (let members
           (lambda args
             (apply
              (case (car args)
                methods
                ...
                (else (error "Method lookup failed")))
              (cdr args))))))))
