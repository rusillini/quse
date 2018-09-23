(almost
 (llrule
  (~ (lexitem (root (in (any all every no none anything nothing everything everyone anyone)))))
  (add-component ($ 2 (litem)) (make-lexitem (low) ($ 1 (high)) (avm syn (Adv))) (str lex-dep)))
 )
(go
 (llrule
  ((word (or (not (in (a the play played plays playing) prep.txt))
	     (str= to)))
   ~)
  (prog
   ($ 2 (keep-tags (pred (att (contain ((cat v)))))))
   )))
(how
 (llrule
  (~ (word (str= to)))
  (prog
   ($ 1 (keep-tags (pred (att (contain (+wh))))))
   ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
   )))
(ago
 (llrule
  ((lexitem (att (or (contain syn (Nadv))
		     (contain syn ((sem (+amount)))))))
   ~)
  (prog
   (delete-if (low) (high) (pred (t)))
   (assert (make-lexitem (low) (high) (avm syn (Nadv (sem (+date))))))
   )))
(can
 (llrule aux-is-followed-by-verb)
 )
(will
 (llrule
  ((lexitem (att (contain ((cat n))))) ~)
  ($ 2 (keep-tags (pred (att (contain ((auxform will))))))))
 (llrule aux-is-followed-by-verb)
 )
(must
 (llrule aux-is-followed-by-verb)
 )
(should
 (llrule aux-is-followed-by-verb)
 )
(shall
 (llrule aux-is-followed-by-verb)
 )
(until
 (llrule
  (~ (lexitem (att (contain ((sem (+date)))))))
  ($ 1 (keep-tags (pred (att (contain ((pform until)))))))
  ))
(back
 (llrule
  (~ (word (in (pain condition pains injury surgery problem muscle
      spasm yard seat injury spasm problem pain surgery tax burner window
      pay side trouble wall page road bedroom pocket row order ailment alley
      rib child support fence foot wage infection patio payment corner deck
      garden label muscle soreness stair terrace bench booth condition court
      facing flip ))))
  (prog
   ($ 1 (keep-tags (pred (att (contain ((cat n)))))))
   ($ 2 (keep-tags (pred (att (contain ((cat n)))))))
   ))
 (llrule
  ((word (not (in (the my your our his her their))))
   ~ (word (and (in prep.txt) (not (str= of)))))
  (prog
   ($ 2 (keep-tags (pred (att (contain ((node PpSpec)))))))
   ($ 3 (keep-tags (pred (att (contain ((cat p)))))))
   ))
 (llrule
  ((word (in (our your their my his her 's '))) ~)
  ($ 2 (keep-tags (pred (att (contain (N)))))))
 (llrule
  ((lexitem (att (and (contain (N)) (unifiable (-genitive))))) ~)
  ($ 2 (keep-tags (pred (att (contain (Adv)))))))
 )
(let
 (llrule
  (~ 's)
  (prog
   (delete-if ($ 2 (low)) (high) (pred (t)))
   (assert 
    (make-lexitem ($ 2 (low)) (high)
		  (avm syn (obj_np Pron -genitive -3sg +plu -prd (per 1)))))))
 )
(subject
 (llrule
  (~ to (lexitem (t)))
  (prog
   ($ 1 (keep-tags (pred (att (contain (Adj))))))
   ($ 2 (keep-tags (pred (att (contain (Prep))))))
   ($ 3 (keep-tags (pred (att (not (contain ((cat v))))))))
   )))
(D.
 (llrule
  ((lexitem (att (contain ((sem (+person))))))
   (word (reg-match [^A-Z]))
   ~ "," (word (reg-match [A-Z])))
  (prog
   (set-high ($ 1 (litem)) (high))
   (delete-smaller (low) (high))
   )
  ))
(R.
 (llrule
  ((lexitem (att (contain ((sem (+person))))))
   (word (reg-match [^A-Z]))
   ~ "," (word (reg-match [A-Z])))
  (prog
   (set-high ($ 1 (litem)) (high))
   (delete-smaller (low) (high))
   )
  ))
(Figure
 (llrule
  (~ (word (reg-match "[0-9.]+")) (word (str= "(")) (word (t)) (word (str= ")")))
  (prog
   (delete-smaller (low) (high))
   (assert
    (make-lexitem (low) (high) (avm syn (PN)))))))
(book
 (llrule book-title))
(novel
 (llrule book-title))
(essay
 (llrule book-title))
(poem
 (llrule book-title))
(biography
 (llrule book-title))
(memoir
 (llrule book-title))
(autobiography
 (llrule book-title))
(monograph
 (llrule book-title))
(play
 (llrule book-title))
(fiction
 (llrule book-title))
(pamphlet
 (llrule book-title))
(novella
 (llrule book-title))
(screenplay
 (llrule book-title))
(manuscript
 (llrule book-title))
(song
 (llrule book-title))
(term
 (llrule term-content1)
 (llrule term-content2))
(nickname
 (llrule term-content1)
 (llrule term-content2))
(name
 (llrule term-content1)
 (llrule term-content2))
(word
 (llrule term-content1))
(slogan
 (llrule term-content1))
(proverb
 (llrule term-content1))
(phrase
 (llrule term-content1))
(available
 (llrule
  (now ~)
  (prog
   (delete-if ($ 1 (low)) ($ 1 (high)) (pred (t)))
   (assert
    (make-lexitem ($ 1 (low)) ($ 1 (high)) (avm syn (Adv)))))))
(trying
 (llrule
  (~ to)
  (prog
   ($ 1 (keep-tags (pred (att (contain ((cat v)))))))
   ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
   )))
(wrong
 (llrule
  ((word (in (something nothing anything))) ~)
  (prog
   ($ 2 (keep-tags (pred (att (contain (A)))))))))
(class
 (llrule
  (~ (word (or (in (I II III IV V VI VII VIII IX X) numbers.txt)
	       (reg-match [0-9]+))))
  (prog
   (delete-smaller (low) (high))
   (assert
    (make-lexitem (low) (high) (avm syn (N)))))))
(author
 (llrule author-of-book))
(coauthor
 (llrule author-of-book))
(writer
 (llrule author-of-book))
(critic
 (llrule author-of-book))
(copy
 (llrule author-of-book))
(draft
 (llrule author-of-book))
(text
 (llrule author-of-book))
(passage
 (llrule author-of-book))
(reading
 (llrule author-of-book))
(version
 (llrule author-of-book))
(interpretation
 (llrule author-of-book))
(spring
 (llrule 
  ((word (in (this each last which annual regular))) ~)
  ($ 2 (keep-tags (pred (att (contain (N)))))))
 (llrule 
  (~ (word (in (camp trap break semester camps traps breaks semesters))))
  ($ 1 (keep-tags (pred (att (contain (N))))))))
(been
 (llrule
  ((word (in (have has had having))) ~)
  ($ 1 (keep-tags (pred (att (contain ((cat i)))))))))
(you
 (llrule
  (~ (word (in (boys girls ladies gentlemen guys kids children))))
  (prog
   (delete-smaller (low) (high))
   (combine-lexitems
    (assert (make-lexitem ($ 1 (low)) ($ 1 (high)) (avm syn (N +pron +plu))))
    lex-dep
    (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn (N)))))))
(try
 (llrule
  (~ (word (str= to)))
  (prog
   ($ 1 (keep-tags (pred (att (contain ((cat v)))))))
   ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
   )))
(help
 (llrule help+verb))
(helps
 (llrule help+verb))
(helped
 (llrule help+verb))
(seed
 (~)
 (delete-if (low) (high) (pred (att (contain (verb (vform ed)))))))
(enough
 (llrule
  (~ of)
  ($ 1 (keep-tags (pred (att (contain (N))))))))
(all
 (llrule
  (~ of)
  ($ 1 (keep-tags (pred (att (contain (N)))))))
 (llrule
  (~ by (word (reg-match .*self)))
  (prog
   (delete-smaller (low) ($ 2 (high)))
   (combine-lexitems
    (assert (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn (Prep (pform by)))))
    lex-dep
    (make-lexitem ($ 1 (low)) ($ 1 (high)) (avm syn (Pron))))
   )))
(by
 (llrule
  (~ (word (in (! . "\"" , ?) prep.txt)))
  (assert (make-lexitem ($ 1 (low)) ($ 1 (high)) (avm syn (Adv))))
  ))
;(where
; (llrule
;  (~ (lexitem (att (contain ((cat a))))))
;  (assert (make-lexitem (low) (high) (avm syn (Adv (node Adjunct)))))))
(so
 (llrule
  ((lexitem (att (contain (Tn)))) ~)
  (assert (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn (N (case acc))))))
 (llrule
  ((lexitem (att (or (contain ((args () ((cat c) (cform fin)))))
		     (contain ((args () () ((cat c) (cform fin))))))))
   ~)
  (assert (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn ((cat c) (node C)))))))
(no
 (llrule
  (~ (word (or (str= ,) (str= !))))
  (prog
   (assert (make-lexitem ($ 1 (low)) ($ 1 (high)) (avm syn (Adv))))
   ($ 1 (keep-tags (pred (att (contain (Adv))))))
   )))
('s
 (llrule
  ((word (not (str= let))) ~)
  ($ 2 (keep-tags (pred (att (not (contain (N)))))))
  )
 )
(what
 (llrule
  (~ (lexitem (att (or (contain (PN))
		       (defined (auxform))))))
  ($ 1 (keep-tags (pred (att (contain (N))))))
  )
 )
(about
 (llrule
  (~ (word (str= to)))
  (prog
   ($ 1 (keep-tags (pred (att (contain (At))))))
   ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
   )))
(very
 (llrule 
  (~ (word (in (active tender young common wrong bad kind simple firm high low patient special fine short small cold familiar large positive quiet shy calm conservative long personal tough smooth preliminary offensive safe old hard alert raw specific sweet clean dependent minor intensive tense brave daring emotional faithful fat))))
  ($ 1 (keep-tags (pred (att (contain (Adv))))))
  )
 )
(leaves
 (llrule
  ((word (in (maternity disability parental unpaid paid))) ~)
  ($ 2 (keep-tags (pred (root (str= leave)))))))
(lean
 (llrule
  (~ (word (str= on)))
  ($ 1 (keep-tags (pred (att (contain (verb))))))))
(expected
 (llrule
  ((word (str= than)) ~ (word (reg-match "[^a-zA-Z0-9]")))
  (assert (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn (N))))
  ))
(willingness
 (llrule
  (~ (word (str= to)))
  ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
  ))
(refusal
 (llrule
  (~ (word (str= to)))
  ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
  ))
(reluctance
 (llrule
  (~ (word (str= to)))
  ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
  ))
(desire
 (llrule
  (~ (word (str= to)))
  ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
  ))
(agreement
 (llrule
  (~ (word (str= to)))
  ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
  ))
(decision
 (llrule
  (~ (word (str= to)))
  ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
  ))
(failure
 (llrule
  (~ (word (str= to)))
  ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
  ))
(ability
 (llrule
  (~ (word (str= to)))
  ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
  ))
(tm
 (llrule
  ((word (t))
   (word (or (str= "(") (str= "[")))
   ~
   (word (or (str= ")") (str= "]"))))
  (assert (make-lexitem (low) (high) (avm syn (N))))
  ))
(too
 (llrule
  (~ (lexitem (att (contain ((cat a))))) (word (str= to)))
  (prog
   ($ 2 (keep-tags (pred (att (contain ((cat a)))))))
   ($ 3 (keep-tags (pred (att (contain ((auxform to))))))))
  ))
(bit
 (llrule
  ((word (str= a)) ~ (lexitem (att (contain ((cat a))))))
  (prog
   (delete-smaller ($ 1 (low)) ($ 2 (high)))
   ($ 3 (keep-tags (pred (att (contain ((cat a))))))))
  ))
;(that
; (llrule
;  ((lexitem (att (contain (verb (args () ((cat c))))))) ~)
;  ($ 2 (keep-tags (pred (att (contain ((cat c)))))))
;  ))
(click
 (llrule
  ((word (str= then)) ~)
  ($ 2 (keep-tags (pred (att (contain ((cat v)))))))
  ))

(bit
 (llrule
  ((word (reg-match [0-9])) ~)
  ($ 2 (keep-tags (pred (att (contain ((cat n)))))))
  ))

(later
 (llrule
  ((lexitem (att (contain ((sem (+amount +spec)))))) ~)
  (prog
   (combine-lexitems (make-lexitem ($ 2 (low)) (high) (avm syn (Adv)))
		     amount-value ($ 1 (litem)))
   (delete-smaller (low) (high)))
  )
 )
(won
 (llrule
  (~ (lexitem (att (contain ((sem (+number)))))))
  ($ 1 (keep-tags (pred (att (contain ((cat v)))))))
  )
 )
(room
 (llrule
  (~ (word (in (keys key))))
  (prog
   ($ 1 (keep-tags (pred (att (contain ((cat n)))))))
   ($ 2 (keep-tags (pred (att (contain ((cat n)))))))
   )))
(like
 (llrule
  ((lexitem (root (str= look))) ~)
  (prog
   ($ 1 (keep-tags (pred (att (contain ((cat v)))))))
   ($ 2 (keep-tags (pred (att (contain ((cat p)))))))
   )))


 