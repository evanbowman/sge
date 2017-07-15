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


(define apartment-0 (Level))

(apartment-0
 'add-entry-hook
 (lambda ()
   (define bkg (entity-create))
   (define fg (entity-create))
   (define sunbeam-0 (entity-create))
   (define sunbeam-1 (entity-create))
   (set-refresh-rgba 20 20 54 255)
   (entity-set-rgba (*player* 'get-handle) 255 255 255 128)
   (entity-set-animation bkg anim-apt-0-bkg)
   (entity-set-animation sunbeam-0 anim-sunbeam-0)
   (entity-set-animation sunbeam-1 anim-sunbeam-1)
   (entity-set-animation fg anim-apt-0-fg)
   (entity-set-zorder sunbeam-0 1)
   (entity-set-zorder sunbeam-1 1)
   (entity-set-zorder fg 1)
   (entity-set-blend-mode sunbeam-0 blend-add)
   (entity-set-blend-mode sunbeam-1 blend-add)
   (entity-set-position sunbeam-0 97 18)
   (entity-set-position sunbeam-1 150 66)
   (entity-set-position fg -15 -16)
   (apartment-0 'add-entity bkg)
   (apartment-0 'add-entity sunbeam-0)
   (apartment-0 'add-entity sunbeam-1)
   (apartment-0 'add-entity fg)
   (*player* 'reset-with-position 87 119)
   (camera-set-center 87 119)))
