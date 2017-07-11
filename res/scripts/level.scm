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



(define apartment-0 (Level))

(define apt-0-bkg-anim
  (animation-create
   "apartment-level-0.png"
   0 0 193 144 0 0))

(apartment-0
 'add-entry-hook
 (lambda ()
   (define bkg (entity-create))
   (entity-set-animation bkg apt-0-bkg-anim)
   (entity-set-scale bkg 2 2)
   (entity-set-position bkg 0 62)
   (apartment-0 'add-entity bkg)
   (*player* 'reset-with-position 60 300)))
