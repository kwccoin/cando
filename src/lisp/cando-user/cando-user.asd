(in-package :asdf-user)

(defsystem "cando-user"
    :description "The CANDO compiler front end"
    :version "0.0.1"
    :author "Christian Schafmeister <chris.schaf@verizon.net>"
    :licence "LGPL-3.0"
    :depends-on (:cando :charges :design :leap :cando-jupyter)
    :serial t
    :components ((:file "packages")
                 (:file "start-swank")
                 (:file "graphviz")
                 (:file "cl-jupyter")))
