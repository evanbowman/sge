;;;
;;; level.scm
;;;
;;; ...
;;;

(define *current-level*
  (lambda (unused-args) '()))

(class Level
  ((entry-hooks '())
   (exit-hooks '())
   (entity-list '())
   (wall-list '()))

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
     (*player* 'update dt)))

  ((enter)
   (lambda ()
     (for-each (lambda (hook) (hook)) entry-hooks)))

  ((exit)
   (lambda ()
     (for-each (lambda (hook) (hook)) exit-hooks))))

(define (switch-level new-level)
  (let ((old-level *current-level*))
    (set! *current-level* new-level)
    (old-level 'exit)
    (new-level 'enter)
    old-level))

(define test-level (Level))

(test-level 'add-entry-hook
            (lambda ()
              (*player* 'reset-with-position 200 300)))
