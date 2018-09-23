(load minipar-init.lsp)
(disable-rules English-lexical-rules hyphen-unknown1)
(insert-lex-rules English-lexical-rules hyphen-unknown1
(hyphen-unknown1-revised
  ;; guess the POS of a hyphenated word
  (seq 
   (+ (seq
       (or-pat (lexitem (att (not (or (contain (Adv))
				      (contain ((cat i)))))))
	       (word (reg-match [A-Za-z0-9])))
       (word (or (str= -) (str= "&ndash;")))
       (? (word (in (, and or))))))
   (word (t)) ;(not (in (a the an - and or , have had) skip.txt auxiliary.txt prons.txt)))
   )
  (hyphen-unknown-func)
  )
)


;(load semannot-init.lsp)

;(make-tripledb db 19997 :clean-up-after 50000) (link-file db trecdep.hdr)(set-reldb English-parser db)
;(make-simworddb simdb :max-syns 200 :size 20011 :db sims.hdr)
;(make-tripledb db 19997 :clean-up-after 50000) (link-file db all.hdr)(set-reldb English-parser db)
;(make-simworddb simdb :max-syns 200 :size 80011 :noun simN.hdr :verb simV.hdr :adj/adv simA.hdr)
;(load csw.lsp)
;(make-pairdb pairdb 200011)(link-file pairdb pairs.hdr)(set-pairdb English-parser pairdb)

