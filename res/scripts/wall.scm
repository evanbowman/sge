;;;
;;; wall.scm
;;;

(define (create-wall x y w h)
  (let ((wall (entity-create)))
    (entity-set-position wall x y)))
