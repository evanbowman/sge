;;;
;;; level.scm
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
     (for-each (lambda (entity) (entity-remove entity)) entity-list)
     (set! entity-list '())
     (for-each (lambda (hook) (hook)) exit-hooks))))

(define (switch-level new-level)
  (let ((old-level *current-level*))
    (set! *current-level* new-level)
    (old-level 'exit)
    (new-level 'enter)
    old-level))

(define apt-0-bkg-anim
  (animation-create
   "apartment-level-0.png"
   0 0 193 144 0 0))

(define sunbeam-0-anim
  (animation-create
   "sunbeam-0.png"
   0 0 96 99 0 0))

(define sunbeam-1-anim
  (animation-create
   "sunbeam-1.png"
   0 0 43 44 0 0))

(define apartment-0 (Level))

(apartment-0
 'add-entry-hook
 (lambda ()
   (define bkg (entity-create))
   (define sunbeam-0 (entity-create))
   (define sunbeam-1 (entity-create))
   (entity-set-animation bkg apt-0-bkg-anim)
   (entity-set-animation sunbeam-0 sunbeam-0-anim)
   (entity-set-animation sunbeam-1 sunbeam-1-anim)
   (entity-set-scale sunbeam-0 2 2)
   (entity-set-scale sunbeam-1 2 2)
   (entity-set-zorder sunbeam-0 1)
   (entity-set-zorder sunbeam-1 1)
   (entity-set-blend-mode sunbeam-0 blend-add)
   (entity-set-blend-mode sunbeam-1 blend-add)
   (entity-set-scale bkg 2 2)
   (entity-set-position bkg 0 62)
   (entity-set-position sunbeam-0 194 92)
   (entity-set-position sunbeam-1 300 220)
   (apartment-0 'add-entity bkg)
   (apartment-0 'add-entity sunbeam-0)
   (apartment-0 'add-entity sunbeam-1)
   (*player* 'reset-with-position 60 300)))
