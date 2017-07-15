;;;
;;; animations.scm
;;;
;;; All the handles to animations used by the game.
;;;

(define anim-player-run
  (animation-create
   "player-run-cycle.png"
   0 0 21 44 10 20))

(define anim-apt-0-bkg
  (animation-create
   "apartment-level-0.png"
   0 0 193 144 0 0))

(define anim-apt-0-fg
  (animation-create
   "apartment-level-0-mask.png"
   0 0 224 176 0 0))

(define anim-sunbeam-0
  (animation-create
   "sunbeam-0.png"
   0 0 96 99 0 0))

(define anim-sunbeam-1
  (animation-create
   "sunbeam-1.png"
   0 0 43 44 0 0))
