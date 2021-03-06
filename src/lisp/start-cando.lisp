;;;
;;;    File: start-cando.lisp
;;;

;; Open Source License
;; Copyright (c) 2016, Christian E. Schafmeister
;; Permission is hereby granted, free of charge, to any person obtaining a copy
;; of this software and associated documentation files (the "Software"), to deal
;; in the Software without restriction, including without limitation the rights
;; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
;; copies of the Software, and to permit persons to whom the Software is
;; furnished to do so, subject to the following conditions:
;; The above copyright notice and this permission notice shall be included in
;; all copies or substantial portions of the Software.
;; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
;; THE SOFTWARE.
;;  
;; This is an open source license for the CANDO software from Temple University, but it is not the only one. Contact Temple University at mailto:techtransfer@temple.edu if you would like a different license.

;; -^-
#+(or)
(eval-when (:load-toplevel :execute :compile-toplevel)
  (format t "Turning on core:*echo-repl-read* in start-cando.lisp~%")
  (setq core::*echo-repl-read* t))

(in-package :common-lisp-user)

(format t "Starting start-cando.lisp script - arguments: ~s~%" core:*command-line-arguments*)

;;; Load the ASDF package manager
(progn
  (format t "Loading asdf~%")
  (require :asdf))
(format t "Loaded asdf version ~s~%" (asdf/upgrade:asdf-version))

(defun all-subdirs (dir)
  (let (dirs)
    (labels ((trav (d)
               (dolist (d (uiop:subdirectories d))
                 (push d dirs)
                 (trav d))))
      (trav dir))
    dirs))

;;; Add the cando hostname
(format t "Setting CANDO hostname~%")
(progn
  (setf (logical-pathname-translations "cando")
        '(("**;*.*" "source-dir:extensions;cando;src;**;*.*"))))

;;; Add directories for ASDF to search for systems
(let* ((topdir (translate-logical-pathname #P"cando:lisp;"))
       (dirs (all-subdirs topdir)))
  (push topdir asdf:*central-registry*)
  (dolist (dir dirs)
    (format t "Pushing dir: ~a~%" dir)
    (push dir asdf:*central-registry*)))

(progn
  (format t "Pushing dir: ~a~%" *default-pathname-defaults*)
  (push *default-pathname-defaults* asdf:*central-registry*))

;;;(make-package :cando)

(let ((amber-home
        (namestring (uiop:ensure-directory-pathname (or (ext:getenv "AMBERHOME") "/amber/")))))
  (setf (logical-pathname-translations "amber")
        (list (list "**;*.*" (concatenate 'string amber-home "/**/*.*"))))
  (format t "Setting *amber-home* -> ~a~%" amber-home))

;;; Setup or startup the Cando system 
;;; If :setup-cando is in *features* then don't load the cando system
(progn
  (format t "Starting Cando~%")
  (format t "*features* -> ~a~%" *features*))

(progn
  (format t "Loading quicklisp.~%")
  (load "quicklisp:setup.lisp"))
(progn
  (format t "Loading cando-user system.~%")
  (funcall (find-symbol "QUICKLOAD" :ql) "cando-user" :verbose t)
  ;; Ensure that all threads start in the :CANDO-USER package
  (core:symbol-global-value-set '*package* (find-package :cando-user)))


