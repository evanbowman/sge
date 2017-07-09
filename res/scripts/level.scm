;;;
;;; level.scm
;;;
;;; ...
;;;

(define *current-level* '())

(class Level
  ((entry-hooks '())
   (exit-hooks '())
   (entity-list '()))

  ((add-entry-hook)
   (lambda (hook)
     (set! entry-hooks (cons hook entry-hooks))))
  
  ((add-exit-hook)
   (lambda (hook)
     (set! exit-hooks (cons hook exit-hooks))))

  ((add-entity)
   (lambda (entity)
     (set! entity-list (cons entity entity-list))))
    
  ((update)
   (lambda (dt)
     '()))

  ((enter) (lambda () '()))

  ((exit) (lambda () '())))

(define (switch-level new-level)
  (let ((old-level *current-level*))
    (set! *current-level* new-level)
    (old-level 'exit)
    (new-level 'enter)
    old-level))
