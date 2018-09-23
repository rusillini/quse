;; lexicalized tagging rules

(insert-lex-rules English-lexical-rules set-flags
 (back
  (seq (lexitem (att (contain (verb))))
       (word (str= back))
       )
  (delete-if ($ 2 (low)) (high) 
	     (pred (att (not (contain ((cat a)))))))
  )
)