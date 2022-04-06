(use-modules  (px packages nheko)
              (gnu packages)
              (gnu packages audio)
              (gnu packages autotools)
              (gnu packages base)
              (gnu packages boost)
              (gnu packages build-tools)
              (gnu packages check)
              (gnu packages cmake)
              (gnu packages compression)
              (gnu packages cpp)
              (gnu packages crypto)
              (gnu packages curl)
              (gnu packages databases)
              (gnu packages documentation)
              (gnu packages enchant)
              (gnu packages fontutils)
              (gnu packages gcc)
              (gnu packages gstreamer)
              (gnu packages graphviz)
              (gnu packages image)
              (gnu packages libevent)
              (gnu packages logging)
              (gnu packages markup)
              (gnu packages messaging)
              (gnu packages networking)
              (gnu packages ninja)
              (gnu packages pkg-config)
              (gnu packages python)
              (gnu packages qt)
              (gnu packages tls)
              (gnu packages xorg)
              (guix build-system cmake)
              (guix build-system gnu)
              (guix build-system meson)
              (guix build-system qt)
              (guix download)
              (guix git-download)
              ((guix licenses) #:prefix license:)
              (guix packages)
              (guix utils)
              (px  packages matrix-client)
              (px  packages qt))

(define-public matrix-client-gui-library
  (package
    (name "matrix-client-gui-library")
    (version "0.0.5")
    (source
     (origin
       (method url-fetch)
        (uri (string-append
          "https://source.pantherx.org/matrix-client_" version ".tgz"))
        (sha256
         (base32 "0kfqbad7qzfiykv1pb87hbh26kskrkl57msczww6rk1wjrilqqb9"))))
    (arguments
     `(#:tests? #f ; no tests
       #:phases
       (modify-phases %standard-phases
         (replace 'configure
           (lambda* (#:key outputs #:allow-other-keys)
             (substitute* "MatrixClientLib.pro"
               (("/usr") (assoc-ref outputs "out")))
             (invoke "qmake" "MatrixClientLib.pro" ))))))
    (build-system qt-build-system)
    (inputs
     `(("coeurl" ,coeurl)
       ("curl" ,curl)
       ("json-modern-cxx" ,json-modern-cxx)
       ("libolm" ,libolm)
       ("lmdb" ,lmdb)
       ("lmdbxx" ,lmdbxx)
       ("mtxclient" ,mtxclient)
       ("matrix-client-library" ,matrix-client-library)
       ("openssl" ,openssl)
       ("qtbase" ,qtbase-5)
       ("qtdeclarative" ,qtdeclarative)
       ("qtgraphicaleffects" ,qtgraphicaleffects)
       ("qtmultimedia" ,qtmultimedia)
       ("qtquickcontrols2" ,qtquickcontrols2)
       ("qtsvg" ,qtsvg)
       ("spdlog" ,spdlog)
       ))
    (native-inputs
     `(("pkg-config" ,pkg-config)
       ("qtlinguist" ,qttools)))
    (home-page "")
    (synopsis "")
    (description "")
    (license license:gpl3+)))

(define-public matrix-client-gui
  (package
    (inherit matrix-client-gui-library)
    (name "matrix-client-gui")
    (version "0.0.5")
    (arguments
     `(#:tests? #f ; no tests
       #:phases
       (modify-phases %standard-phases
         (replace 'configure
           (lambda* (#:key outputs #:allow-other-keys)
             (substitute* "MatrixClientApp.pro"
               (("/usr") (assoc-ref outputs "out")))
             (invoke "qmake" "MatrixClientApp.pro" ))))))))
    
matrix-client-gui-library
; matrix-client-gui