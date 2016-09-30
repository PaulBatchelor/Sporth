(define (parse s lst)
  (map (lambda (x) (ps-lexer s x)) lst))

(define (ps-val x)
  (if (string? x)
    (car (cons x '()))
    (number->string x)))

(define (ps-append lst) 
  (if (list? lst) 
    (map ps-val lst)
    (list (ps-val lst))))

(define (sine frq amp) (append (ps-append frq) (ps-append amp) (list "sine"))) 
(define (add v1 v2) (append (ps-append v1) (ps-append v2) (list "+")))
(define (div v1 v2) (append (ps-append v1) (ps-append v2) (list "/")))
(define (sub v1 v2) (append (ps-append v1) (ps-append v2) (list "-")))
(define (mul v1 v2) (append (ps-append v1) (ps-append v2) (list "*")))
(define (tget tbl indx)
  (append 
    (ps-append indx)
    (ps-append tbl)
    (list "tget")))

(define (tenv t a h r)
  (append (ps-append t) (ps-append a) (ps-append h) (ps-append r) '("tenv")))

(define (tenvx t a h r)
  (append (ps-append t) (ps-append a) (ps-append h) (ps-append r) '("tenvx")))

(define (bpm2dur bpm) (append (ps-append bpm) '("bpm2dur")))

(define (fm frq amp cr md idx)
  (append (ps-append frq)
          (ps-append amp)
          (ps-append md)
          (ps-append cr)
          (ps-append idx)
          '("fm")))

(define (mtof m) (append (ps-append m) '("mtof")))

(define (port in frq) (append (ps-append in) (ps-append frq) '("port")))

(define (quotes s) (string-append "'" s "'"))

(define (diskin s)
  (append (ps-append s) '("diskin")))

(define (cdbload f s)
  (append (ps-append f) (ps-append (quotes s)) '("cdbload")))

(define (cdb f key)
  (append (ps-append (quotes key)) (ps-append f) '("cdb")))

(define (gen-composite f n s)
  (append (ps-append f) (ps-append n) (ps-append s) '("gen_composite")))

(define (loadfile f s)
  (append (ps-append f) (ps-append s) '("loadfile")))

(define (phasor phs f)
  (append (ps-append f) (ps-append phs) '("phasor")))

(define (tabread ft scaled off wrap idx)
  (append
    (ps-append idx)
    (ps-append scaled)
    (ps-append off)
    (ps-append wrap)
    (ps-append ft)
    '("tabread")))

(define (tbldur ft) (append (ps-append ft) '("tbldur")))
(define (dup x) (append (ps-append x) '("dup")))
(define (swap x y) (append (ps-append x) (ps-append y) '("swap")))
(define (drop x) (append (ps-append x) '("drop")))
(define (trand t mn mx) (append 
                          (ps-append t) 
                          (ps-append mn) 
                          (ps-append mx)
                          '("trand")))

(define (fog amp dense trans spd oct band ris dec dur phs olaps win wav)
  (append 
    (ps-append amp)
    (ps-append dense)
    (ps-append trans)
    (ps-append spd)
    (ps-append oct)
    (ps-append band)
    (ps-append ris)
    (ps-append dec)
    (ps-append dur)
    (ps-append phs)
    (ps-append olaps)
    (ps-append win)
    (ps-append wav)
    '("fog")))

(define (floor x) (append (ps-append x) '("floor")))
(define (biscale sig mn mx)
  (append (ps-append sig) (ps-append mn) (ps-append mx) '("biscale")))

(define (scale sig mn mx)
  (append (ps-append sig) (ps-append mn) (ps-append mx) '("scale")))

(define (randi mn mx rt)
  (append
    (ps-append mn)
    (ps-append mx)
    (ps-append rt)
    '("randi")))

(define (randh mn mx rt)
  (append
    (ps-append mn)
    (ps-append mx)
    (ps-append rt)
    '("randh")))


(define (dmetro dur) (append (ps-append dur) '("dmetro")))
(define (timer tk) (append (ps-append tk) '("timer")))
(define (inv v) (append (ps-append v) '("inv")))
(define (clockmult tick m) (append (dmetro (div (timer tick) m))))

(define (granulate env wav spd amp pos) 
(append 
  (gen-composite env 4096 (quotes "0.5 0.5 270 0.5"))
  (fog amp 80 spd pos 0 12 0.1 0.1 0.2 0 80 "_env" "_wav")))

(define (ps-nt vg st dur args)
  (apply ps-note 
         (append (list 
                   (vector-ref vg 0)
                   (vector-ref vg 1)
                   st 
                   dur dur) args)))

(define (ps-multi-eval beg end str)
  (if (> beg end)
    ()
    (begin 
      (ps-eval beg str)
      (ps-multi-eval (+ beg 1) end str))))

(define (ps-multi-parse beg end str)
  (if (> beg end)
    ()
    (begin 
      (parse beg str)
      (ps-init-sporthlet beg)
      (ps-multi-parse (+ beg 1) end str))))

; Assumes in table is called in and set to dmetro param
(define (ps-get-dur) (mul (tget "_args" 0) (tget "_in" 0)))
