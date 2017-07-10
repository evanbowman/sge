;;;
;;; class.scm
;;;
;;; A class in the game's code is just a convenience
;;; macro for grouping functions with data.
;;;

(define-syntax class
  (syntax-rules ()
    ((class name members methods ...)
     (define (name)
       (let self members
           (lambda args
             (apply
              (case (car args)
                methods
                ...
                (else (error "Method lookup failed")))
              (cdr args))))))))
