;;; mind.el --- An Emacs mode for mind programs
;; Copyright 2011-2013 Markus Redeker <cep@ibp.de>
;;
;; Published under the GNU General Public License version 2 or any
;; later version, at your choice. There is NO WARRANY, not at all. See
;; the file "copying" for details.

;; This is a simple mode to edit mind programs.
;;
;; To enable it, put this directory into the search path and load it,
;; with code like this in your .emacs file:
;;
;;    (add-to-list 'load-path "/your/path/to/mind/support")
;;    (require 'mind)

;;; Code:

(define-derived-mode mind-mode prog-mode "mind"
  "Mode for mind programs"
  (setq comment-start "\\ ")
  (setq comment-start-skip "\\\\")
)

(add-to-list 'auto-mode-alist '("\\.mind$" . mind-mode))

(provide 'mind)
;;; mind.el ends here
