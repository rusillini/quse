(make-word-list nn :hashsize 19997 nn.txt)
(make-word-list adj-noun :hashsize 19997 adj-noun.txt)

(defmacro lexitemlist-hook ()
 (prog
;  (when (att
;	 (and (not (defined (node))) ;; to prevent lexitems with (node NN) 
;	                             ;; from being registered
;	      (value sem "AttVec*" 
;		     (defined-one-of sem (corpname person location)))))
;    (cond
;     ((att (contain syn ((sem (+person)))))
;      (register-name the-known-persons))
;     ((att (contain syn ((sem (+corpname)))))
;      (register-name the-known-organizations))
;;     ((att (contain syn ((sem (+location)))))
;;      (register-name the-known-locations))
;     ))
  (set-head-of-lexitem)
  )
)

(defun hyphen-unknown-func ()
  (when (not (or (exist-lexitem (low) ($ 2 (high)))
		 (followed-by-word (pred (str= -)))))	     
    (let (first-word new-item)
      (setq first-word ($ 1 ($ 1 ($ 1 (matching-word)))))
      (setq new-item
	    (cond 
	     ((get-val first-word (in (ex sub)))
	      (make-lexitem (low) ($ 2 (high)) (avm syn (N)))
	      )
	     ((get-val first-word (in (anti extra)))
	      (make-lexitem (low) ($ 2 (high)) (avm syn (Adj)))
	      )
	     ((get-val first-word (in (re)))
	      (make-lexitem (low) ($ 2 (high)) (avm syn (T[n])))
	      )
	     (($ 2 (matching-word (in (about above across after along at between by down for in inside into of off on out over to up with without))))
	      (if ($ 1 (matching-word (reg-match ".*ed ?-")))
		  (make-lexitem (low) ($ 2 (high)) (avm syn (Adj)))
		(make-lexitem (low) ($ 2 (high)) (avm syn (N))))
	      )
	     (($ 2 (matching-word
		    (or (reg-match [A-Z0-9])
			(suffix (s)))))
	      (if ($ 2 (matching-word (suffix (s))))
		  (make-lexitem (low) ($ 2 (high)) (avm syn (N +plu)))
		(make-lexitem (low) ($ 2 (high)) (avm syn (N))))
	      )
	     (($ 2 (matching-word (in (wide))))
	      (make-lexitem (low) ($ 2 (high)) (avm syn (A))))
	     ((followed-by (pred (att (contain syn ((cat n))))))
	      (make-lexitem (low) ($ 2 (high)) (avm syn (Adj)))
	      )
	     ((t)
	      (make-lexitem (low) ($ 2 (high)) (avm syn (N)))
	      )))
      (set-head
       (get-val new-item)
       ($ 2 (high)))
;      (if ($ 1 ($ 1 ($ 1 (litem))))
;	  (add-matched-lexitems (get-val new-item))
;	)
	   
      (assert (get-val new-item))
;      (add-components
;       (get-val new-item)
;       (low) ($ 2 (high))
;       (pred
;	(att 
;	 (value 
;	  sem "AttVec*"
;	  (defined-one-of sem 
;	    (money time date corpname percent location city province person title country product spec))))))
      (if (not (exist-overlapping-lexitem (low) ($ 2 (high))))
	  (delete-smaller (low) ($ 2 (high))))))
)

(def-rules English-lexical-rules

(all-lower-case
 (seq (beginning-of-sentence)
      (+ (word (reg-match "[^A-Z]")))
      (end-of-sentence))
 (analyzer (set-mixed-case 0)))

(all-upper-case
 (seq (beginning-of-sentence)
      (+ (word (or (reg-match "[^a-z]")
		   (in (
and or the a an aboard about above across after against along
alongside amid amidst among amongst as at before behind below beneath
beside besides between beyond but by despite down during except for
from in inside into less like near next of off on onto out outside
over past pending per plus re round since through throughout thru till
to toward towards under underneath unlike until unto up upon versus
via vice with within without other
)))))
      (end-of-sentence))
 (analyzer (set-mixed-case 0)))

;(debug :act-on-all-matches (lexitem (t)) (nil))

(remove-punctuation-mark-from-word
  (seq (word (and (reg-match ".+[.,;?!][^a-zA-Z0-9]*$")
		  (not (reg-match "&.*;$"))))
       (or-pat (word (t) (reg-match "[^a-zA-Z0-9]"))
	       (end-of-sentence)))
  (prog
   (remove-punctuation-marks ($ 1 (high)))
   (look-up (low) ($ 1 (high)))
   )
  )

(initial-quote
 (seq (beginning-of-sentence)
      (word (reg-match '))
      )
 (look-up ($ 2 (low)) (high) (matching-word (reg-sub " *'" "")))
 )

;;(secretary
;; (lexitem (and (att (contain ((sem (+person)))))
;;	       (root (reg-match "Secretary of " ".* Secretary$"))))
;; (prog
;;  (cond ((litem (root (reg-match "Secretary of ")))
;;	 (add-component (litem) (make-lexitem (low (+1 (+1))) (high) (avm syn (PN (sem (+corpname))))))
;;	 )
;;	((litem (root (reg-match ".* Secretary$")))
;;	 (add-component (litem) (make-lexitem (low) (high (-1)) (avm syn (PN (sem (+corpname))))))
;;	 )
;;	)
;;  (litem (att (undefine (sem))))
;;  (delete-smaller (low) (high)))
;; )

(n't
  ;; if a word x is followed by 't, look up the lexicon for x't
  ;; for example, hasn't, aren't
  (seq (word (t)) (word (or (str= 't) (str= n't))))
  (prog
   (look-up (low) (high))
   (delete-smaller (low) (high))))

(abbreviation
 (seq (word (or (str= "(") (str= "[")))
      (seq 
       (word (reg-match "[A-Za-z].")) ;; at least two letters
       (? (word :nogap (str= -)))
       (? (word :nogap (reg-match "[A-Za-z]")))
       )
      (word (or (str= ")") (str= "]"))))
 (assert (abbrev-def (avm syn (N)))))

(abbreviation-with-definition
 (seq (word (reg-match "[A-Z]."))
      (word (or (str= "(") (str= "[")))
      (+ (word (not (str= "(") (str= "["))))
      (word (or (str= ")") (str= "]"))))
 (assert (abbrev-with-def (avm syn (N)))))

(novel-noun
  ;; if these determiners are followed by a word followed by
  ;;  a preposition, then the word must be a noun.
  (seq :front-to-back
       (word (in (a an the any another every both their its)))
       (* (word (in skip.txt)))
       (and-pat
	(word (reg-match [a-z]))
	(no (lexitem (att (contain syn (N))))))
       (or-pat
	(seq
	 (word (in (and or ,)))
	 (no (lexitem (att (contain syn (A))))))
	(lexitem (or (att (contain syn ((cat p))))
		     (root (in (button key is are were was be have had having has) auxiliary.txt))))
	(end-of-sentence)))
  (assert (make-lexitem ($ 3 (low)) ($ 3 (high)) (avm syn (N)))))

  
(hyphen
  ;; The pattern matches a hyphenated word, which is broken up into
  ;; several words, e.g., forget - me - not
  (seq (word (reg-match [A-Za-z0-9]))
       (+ (seq (word :nogap (or (str= -) (str= "&ndash;")))
	       (word :nogap (and (reg-match [A-Za-z0-9])
				 (not (in (a the an - and or ,))))))))
  (prog
   (look-up (low) (high))
   (when (exist-lexitem (low) (high))
     (delete-smaller (low) (high)))))

(nil-to
  (lexitem (att (contain (+nilTo))))
  (assert
   (make-lexitem (high (+1)) (high)
		 (avm syn ((auxform to) (cform inf) ~tense -govern (node Aux))))))

(debug :act-on-all-matches (lexitem (t)) (nil))

(irregular-forms1 :act-on-all-matches
 (lexitem (and (att (contain syn (+ref)))
	       (not (ending bare))))
 (delete (litem))
 )

(irregular-forms2 :act-on-all-matches
 (lexitem (and (att (contain syn (+ref)))
	       (ending bare)))
 (prog
  (ref-entry (litem))
  (delete (litem))
  )
 )


(post 
  (lexitem (and (att (contain syn ((sem (+post)))))
		(cap)
		(ending bare)))
  (delete-if (low) (high) 
	     (pred (not (att (contain syn ((sem (+post)))))))))


(PN ;; tagging rule
  ;; If a lexical item has +cap attribute, and the actual words in
  ;; the sentence is capitalized, and the word is not the first word
  ;; in the sentence, then delete the meanings of the word that do not
  ;; have +cap attribute.

  :precondition (is-mixed-case)
  (seq (word (reg-match [a-zA-Z]))
       (lexitem (and (att (contain syn (+cap))) ;;not the first word
		     (ending bare)
		     (cap))))
  (delete-if ($ 2 (low)) (high) (pred (att (unifiable (-cap ~sem))))))

(capitalization ;; tagging rule
  ;; If a lexical item has +cap attribute but the corresponding words
  ;; are not capitalized, delete this meaning of the word.
  ;; For example, ME can be a proper noun (abbreviation for Maine),
  ;; but if it appears in the sentence as 'me', then delete the
  ;; proper noun interpretation of the word.
  :precondition (is-mixed-case)
  (lexitem (and (not (cap))
		(att (contain syn (+cap)))))
  (delete (litem)))

(state-abbrev
  ;; If these words appear in the beginning of the sentence or are
  ;; not preceeded by a comma, then they are not state name abbreviations.
  (and-pat (lexitem (att (contain (PN (sem (+province))))))
	   (word (in (IN OK OR OH AL ME))))
  (when (not (and (followed-by-word (pred (in (, -))))
 		 (preceded-by-word (pred (str= ,)))))
    (delete (litem))))

(cap-words-are-nouns
  ;; Delete verb meanings of a capitalized word if it is not inside
  ;; the head.

  :precondition (is-mixed-case)

  (seq (word (and (not (in skip.txt))
		  (reg-match [a-z])))
       (lexitem (and (surform (reg-match .*[A-Z]))
		     (att (or (contain syn ((cat v)))
			      (contain syn ((cat i))))))))
  (delete ($ 2 (litem))))

(novel-proper-nouns
  :precondition (is-mixed-case)
  (word (and (reg-match [A-Z]) (not (in (the an a do did does done have had has was is be am are were been if) prep.txt prons.txt auxiliary.txt))))
   
  (when (and (not (exist-covering-lexitem
		   (low) (high) 
		   (pred (att (contain syn (N))))))
	     (exist-word-in-sentence 
	      (int 1) (low (-1)) (pred (reg-match [A-Za-z]))))
    (assert
     (make-lexitem (low) (high) (avm syn (PN))))))

;(that ;; tagging rule
;  (seq
;   (lexitem (att (contain syn ((args ((cat c) (cform fin)))))))
;   (word (str= that)))
;  (delete-if ($ 2 (low)) ($ 2 (high)) (pred (not (att (contain syn ((cat c)))))))
;  )
   
;(p-trace ;; tagging rule
;  ;; if a preposition is followed by a determiner or a noun, then
;  ;; assume that the object of the preposition is not moved away.
;  (lexitem (att (contain syn ((cat p)))))
;  (when (followed-by (pred (att (or (contain syn ((cat d)))
;				    (contain syn ((cat n)))))))
;    (litem (att (unify (-trace))))))

(not
  ;; If an INFL word is followed by 'not', incorporate the word 
  ;; 'not' into the lexical item for the INFL word.
  (seq
   (lexitem (att (or (contain syn ((node Aux)))
		     (contain syn (+be)))))
   (lexitem (and (root (str= not)) (ending bare)))
   )
  (prog
   ($ 1 (litem (att (unify syn (+neg)))))
   (combine-lexitems ($ 1 (litem)) neg ($ 2 (litem)))
   (delete-smaller (low) (high))))
  
(ending-constraints :act-on-all-matches
  (lexitem (or (not (ending bare))
	       (and (ending bare)
		    (att (contain syn ((cat v)))))))
  (cond
   ((litem (and (ending bare)
		(not (att (contain syn ((cat v) (vform bare ed)))))
		(att (contain syn ((cat v))))))
    (litem (att (unify syn ((vform bare)))))
    )
   ((litem (ending -s))
    (cond ((litem (att (contain syn ((cat v)))))
	   (litem
	    (att (modify
		  ((tense present) (vform s) (cform fin) -plu (per 3)))))
	   )
	  ((litem (att (and (contain syn ((cat n)))
			    (unifiable ((nform norm) +plu -pron -pn)))))
	   (litem (att (unify (+plu +ct))))
	   )
	  ((t)
	   (delete (litem))))
    )
   ((litem (ending -ed))
    (cond ((litem (att (or (contain syn ((cat v)))
			   (contain syn ((cat i))))))
	   (litem (att (unify syn ((vform ed)))))
	   )
	  ((t)
	   (delete (litem))))
    )
   ((litem (ending -ing))
    (cond ((litem (att (or (contain syn ((cat v)))
			   (contain (+have)))))
	   (litem (att (modify syn ((vform ing) +prog -passive))))
	   )
	  ((t)
	   (delete (litem))))
    )
   ((litem (and (or (ending -est) (ending -er))
		(or (not (att (contain syn ((cat a)))))
		    (att (contain (+adv)))
		    (not (att (unifiable (+cmp)))))))
    (delete (litem))
    )
   ((litem (and (ending -dot)
		(not (att (contain syn ((cat n)))))))
    (delete (litem))
    )
   ))

(del-smaller ;; tagging rule
  ;; If a lexical item contains features such as phrase or sem, delete
  ;; smaller lexical items.
  :act-on-all-matches
  (lexitem (or (and
		(att (and (defined (sem)) (not (defined (rare)))))
		(ending bare))
	       (int-gt (lexitem-length) (int 2))
	       (att (defined (phrase)))))
  (when (not (exist-overlapping-lexitem (low) (high) (pred (t))))
   (delete-smaller (low) (high))
   (if (and (matching-word (reg-match [^a-z]))
	    (litem (att (defined (sem)))))
       (delete-if (low) (high) (pred (att (not (defined (sem)))))))
   ))

(number
  ;; Deal with numbers. Examples:
  ;; 	56
  ;; 	about fifty five
  ;; 	one hundred and ten
  ;; 	56-70 
  (seq (? (or-pat (word (in (next previous past other nearly roughly almost about over another)))
		  (word-seq close to)
		  (word-seq up to)
		  (word-seq at least)
		  (word-seq more than)
		  (word-seq less than)))
       (* (seq
	   (word (or (in numbers.txt) (reg-match "[.]?[0-9][0-9.]*$")))
	   (* (word (in (- an a and vs v.s. v.s to through))))))
       (word (or (in numbers.txt) (reg-match "[0-9][0-9.]*$")))
       )
  (cond (($ 1 (matching-word))
	 (assert
	  (combine-lexitems
	   (make-lexitem ($ 2 (low)) (high) (avm syn (N -genitive :opt :of (sem (+number)))))
	   num-mod (make-lexitem ($ 1 (low)) ($ 1 (high)) (avm syn (Aatt)))))
	 (delete-smaller (low) (high))
	 )
	((t)
	 (assert (make-lexitem ($ 2 (low)) (high) (avm syn (N -genitive :opt :of (sem (+number))))))
	 (delete-smaller ($ 2 (low)) (high))
	 )
	)
  )

;(number2
;  ;; Deal with numbers. Examples:
;  ;; 	56
;  ;; 	about fifty five
;  ;; 	one hundred and ten
;  ;; 	56-70 
;  (seq (? (or-pat (word (in (next other nearly roughly almost about over)))
;		  (word-seq up to)
;		  (word-seq more than)
;		  (word-seq less than)))
;       (? (seq
;	   (word (reg-match "[0-9][0-9,/]*[.]?[0-9]*$"))
;	   (* (word (in (- to))))))
;       (word (reg-match "[0-9][0-9,/]*[.]?[0-9]*$"
;			"[.]?[0-9]+$"
;			"[0-9]+th" 
;			"[0-9]*1st"
;			"[0-9]*2nd"
;			"[0-9]*3rd"
;			))
;       )
;  (prog
;    (delete-if (low) (high) (pred (att (contain ((cat n))))))
;    (assert
;     (make-lexitem 
;      (low) (high) (avm syn (N -genitive :opt :of (sem (+number))))))))

(amount
  ;; An amount is a number followed by a unit.
  (seq (lexitem (att (contain syn ((sem (+number))))))
       (? (word (in (a an per -))))
       (lexitem (and (att (contain syn (N)))
		     (root (in units.txt)))))
  (prog
;    ($ 3 (litem (att (unify syn ((sem (+amount +spec)))))))
    (assert
     (combine-lexitems
      (make-lexitem ($ 3 (low)) ($ 3 (high)) (avm syn (N (sem (+amount +spec)))))
      amount-value ($ 1 (litem)))
     )
    ))

;(amount
;  ;; An amount is a number followed by a unit.
;  (seq (? (word (in (last next previous about past almost nearly over))))
;       (+ (lexitem (att (contain syn ((sem (+number)))))))
;       (? (word (in (a an -))))
;       (lexitem (and (att (contain syn (N)))
;		     (root (in units.txt)))))
;  (when (or ($ 1 (matching-word))
;	    ($ 2 ($ -1 (matching-word (not (in (a an)))))))
;    ($ 4 (litem (att (unify syn ((sem (+amount +spec)))))))
;    (set-low ($ 4 (litem)) (low))
;    (delete-smaller (low) (high))))

(clean-up-hyphen
  ;; The number rule will treat a hyphen as a number this rule is to 
  ;; remove it.
  (+ (word (or (str= -) (str= "&ndash;"))))
  (delete-if (low) (high) (pred (t)))
  )

(money1
  ;; references to money with currency symbols, e.g.,
  ;; 	$45
  ;; 	# 45.3 million
  (seq (? (word (in (last next previous about past almost nearly over))))
       (? (word (and (reg-match [A-Z])
		     (not (in begwords.txt)))))
       (or-pat (seq (word (or (reg-match [$#])
			      (in moneyunit.txt)))
		    (or-pat (word (reg-match [0-9]))
			    (lexitem (att (contain syn ((sem (+number))))))))
	  (word (reg-match "[A-Z]*[$#][0-9][0-9,]*[.]?[0-9,]*$")))
       (? (word (in (hundred million trillion billion thousand))))
       )
  (prog
   (delete-smaller (low) (high))
   (assert (make-lexitem (low) (high) (avm syn (N (sem (+money))))))))

(money2
  ;; references to money with currency names, e.g.,
  ;; 	45 New Taiwan Dollars
  ;;    Twelve hundred and thirty one pounds
  (seq (? (word (reg-match [$#])))
       (or-pat (word (reg-match [0-9][0-9]*[.]?[0-9]*$))
	       (lexitem (att (contain syn ((sem (+number)))))))
       (? (word (in (hundred trillion million billion thousand))))
       (* (word (in moneymod.txt)))
       (word (in moneyunit.txt)))
  (prog
   (delete-smaller (low) (high))
   (assert (make-lexitem (low) (high) (avm syn (PN (sem (+money))))))))

(money3
  ;; reference to a range of money, e.g., 3 to 5 dollars
  (seq (lexitem (att (contain syn ((sem (+number))))))
       (word (str= to))
       (lexitem (att (contain syn ((sem (+money)))))))
  (prog
   (combine-lexitems
    ($ 3 (litem))
    (make-lexitem ($ 1 (low)) ($ 1 (high)) (avm syn (PN (sem (+money)))))
    )
   (delete-smaller (low) (high))))

(currency-equivalence
 (seq (lexitem (att (contain syn ((sem (+money))))))
      (word (in skip.txt))
      (lexitem (att (contain syn ((sem (+money))))))
      (word (in skip.txt)))
  (prog
   (set-high
    (combine-lexitems
     ($ 1 (litem))
     ($ 3 (litem))
     )
    (high))
   (delete-smaller (low) (high)))) 

(price
 (seq (lexitem (att (or (contain syn ((sem (+money))))
			(contain syn ((sem (+amount)))))))
      (? (word (str= -)))
      (word (in (an a one each)))
      (? (word (str= -)))
      (word (in units.txt)))
 (when (not (followed-by-word (pred (str= ago))))
  (add-components
   (make-lexitem (low) (high) (avm syn (PN (sem (+price)))))
   (low)
   (high)
   (pred (att (or (contain syn ((sem (+money))))
		  (contain syn ((sem (+amount))))))))
  (delete-smaller (low) (high)))
 )

(date-misc1
  ;; fiscal 1995
  (seq (? (word (in (fiscal late this next last))))
       (word (reg-match "[12][0-9][0-9][0-9][.s]?$" "'[0-9][0-9][.s]?$"))
       (? (word (str= 's)))
       )
  (prog
    (delete-if (low) (high) (pred (t)))
    (assert (make-lexitem (low) (high) (avm syn (PN +adv (sem (+date))))))))

(date-misc
  (seq (? (seq (word (str= fiscal))
	       (? (word (str= -)))))	  
       (word (and (or (reg-match [0-9])
		      (in numbers.txt))
		  (suffix (th irst 1st nd rd))))
       (* (seq (word (str= -))
	       (word (suffix (th irst 1st nd rd -)))))
       (word (in (century half period month year week quarter))))
  (prog
    (delete-if (low) (high) (pred (t)))
    (assert (make-lexitem (low) (high) (avm syn (PN +adv (sem (+date))))))))

(slash-date
 (or-pat
  (word (reg-match [0-9][0-9]/[0-9][0-9]/[0-9][0-9]))
  (seq (word (reg-match [0-9][0-9]$))
       (word (str= -))
       (word (reg-match [0-9][0-9]$))
       (word (str= -))
       (word (reg-match [0-9][0-9]$))
       ))
  (assert (make-lexitem (low) (high) (avm syn (PN +adv -det (sem (+date))))))
 )

(date
  (seq (? (or-pat
	   (seq (word (in dates.txt))
		(word (str= ,)))
	   (word-seq mid -)
	   (word (reg-match [1-9][0-9]?))))
       (word (in months.txt))
       (? (word (reg-match [1-9][0-9]?)))
       (? (seq (word (in (, -)))
	       (word (reg-match [0-9][0-9][0-9]?))))
       (? (word (str= ,)))
       (? (seq (word (str= -))
	       (word (reg-match [0-9][0-9][0-9]?)))))
  (when (or ($ 2 (matching-word (reg-match [A-Z])))
	    ($ 1 (matching-word))
	    ($ 3 (matching-word))
	    ($ 4 (matching-word)))
    (delete-if (low) (high) (pred (t)))
    (assert (make-lexitem (low) (high) (avm syn (PN +adv -det (sem (+date))))))))
  
(date-period
 (seq (word (str= from))
      (lexitem (att (contain (PN (sem (+date))))))
      (word (in (to -)))
      (lexitem (att (contain (PN (sem (+date))))))
      )
 (prog
  (delete-smaller (low) (high))
  (assert (make-lexitem (low) (high) (avm syn (PN +adv (sem (+date))))))))

;(date-ended
;  (seq (+ (word (in (months years quarters weeks one two three four five six seven eight nine ten eleven twelve half first second third fourth fiscal year month quarter week season , that))))
;       (word (str= ended))
;       (? (word (in (last next in on))))
;       (lexitem (att (contain syn ((sem (+date)))))))
;  (prog
;   (delete-if (low) (high) (pred (t)))
;   (assert (make-lexitem (low) (high) (avm syn (PN (sem (+date))))))))

(time1
  (seq (or-pat (seq (+ (word (reg-match "[0-9][0-9]?(:[0-9][0-9])?$")))
		    (? (word (in (am a.m. pm p.m. a.m p.m)))))
	       (word (reg-match "[0-9][0-9][0-9][0-9](EST|CST|MST|PST|GMT|PDT|EDT|CDT|MDT)"))
	       (seq (word (t)) (word (str= o)) (word (str= 'clock)))
;	       (word (in (noon midnight)))
	       )
       (?
	(or-pat (seq (word (t))
		     (? (word (t)))
		     (word (reg-match [Tt]ime$)))
		(word (in (ET EST CST MST PST GMT PDT EDT CDT MDT))))))
  (when ($ 1 (matching-word (reg-match .*[.:'] .*am .*pm .*T$)))
    (delete-smaller (low) (high))
    (assert (make-lexitem (low) (high) (avm syn (PN +adv (sem (+time))))))))
  
;(time2
; (seq (? (word (in (earlier late later early before past))))
;      (lexitem (att (contain (N (sem (+date))))))
;      (lexitem (and (root (in (night afternoon morning noon evening)))
;		    (att (contain (N))))))
; (prog
;  (cond (($ 1 (matching-word))
;	 ($ 3 (litem (att (unify ((sem (+time)))))))
;	 (set-low ($ 3 (litem)) (low)))
;	((t)
;	 (combine-lexitems
;	  ($ 3 (litem))
;	  ($ 2 (litem)))))
;  (delete-smaller (low) (high))
; ))

;(time3
; (seq (? (word (in (earlier late later early before past))))
;      (? (word (in (this that next last))))
;      (or-pat
;       (word (in (night afternoon midafternoon day dawn dusk morning noon nightfall 
;		  today yesterday evening week month year decade century)))
;       (word (and (in months.txt) (reg-match [A-Z])))
;       (lexitem (att (or (contain ((sem (+date))))
;			 (contain ((sem (+time))))))))
;      (? (word (in (later earlier))))
;      )
; (when (or (and ($ 3 (litem))
;		(or ($ 1 (matching-word)) ($ 2 (matching-word)) ($ 4 (matching-word))))
;	   (and (not ($ 3 (litem)))
;		(not (and ($ 3 (matching-word (in (night day noon))))
;			  (not ($ 2 (matching-word)))))))
;   (delete-smaller (low) (high))
;   (cond (($ 3 (litem))
;	  (assert (make-lexitem (low) (high) ($ 3 (meaning))))
;	  )
;	 (($ 3 (matching-word (in (today yesterday week month year decade century) months.txt)))
;	  (assert (make-lexitem (low) (high) (avm syn (PN +adv (sem (+date))))))
;	  )
;	 ((t)
;	  (assert (make-lexitem (low) (high) (avm syn (PN +adv (sem (+time))))))))
;   )
; )
;


(percent
  ;; recognizing percent
  (seq (? (word (str= about)))
       (? (or-pat (word (reg-match "[0-9]+$"))
		  (lexitem (att (contain syn ((sem +number)))))))	  
       (or-pat (word (or (inverse (reg-match [.]?%)) (str= percent)))
	       (word-seq per cent)))
  (prog
   (delete-if (low) (high) (pred (t)))
   (assert (make-lexitem (low) (high) (avm syn (N +group (sem (+percent))))))))

;(plural-has-separate-entry
; (and-pat (lexitem (and (att (contain syn ((cat n))))
;			(ending bare)))
;	  (lexitem (and (att (contain syn ((cat n))))
;			(ending -s))))
; (delete-if (low) (high) (pred (ending -s)))
; )

; (prog
;  (delete-if (low) (high) (pred (and (att (contain syn ((cat n))))
;				     (ending -s))))
;  (litem (att (unify syn (+plu +ct))))))


;(debug-rules compound-noun)

(be+ing-or-ed ;; tagging rule
  ;; if a form of 'be' is followed by a word with endings -ing
  ;; or -ed, then delete the non-verb meanings of the word.
  (seq :front-to-back
   (word (in (is am are be been)))
   (and-pat (word (suffix (ing ed)))
	    (lexitem (att (contain syn ((cat v)))))))
  (delete-if ($ 2 (low)) ($ 2 (high))
	     (pred (att (not (or (contain syn ((cat i)))
				 (contain syn ((cat v)))))))))


(del-verbs ;; tagging rule
  ;; these determiners cannot be followed by a verb.
  (seq 
       (word (in (a an the any every)))
;       (* (lexitem (and (att (contain syn ((cat a))))
;			(no-alternative (att (contain syn (N)))))))
       (lexitem (att (contain syn ((cat v))))))
  (delete ($ 2 (litem))))

(have-been ;; tagging rule
  ;; 'have' followed by 'been' is not a verb
  (seq (word (in (have has had having)))
       (word (str= been))
       )
  (delete-if (low) ($ 1 (high)) (pred (att (not (contain ((cat i))))))))

(corp-name-recognition1
  ;; Combine an lexical item for a company name and a corparate designator
  ;; into a single company name.
  (seq
   (lexitem (and (att (contain syn ((sem (+corpname)))))
		 (cap)))
   (lexitem (att (contain syn ((sem (+corpdesig)))))))
  (prog
   (assert (make-lexitem (low) (high) (avm syn (PN (sem (+corpname))))))
   (delete-smaller (low) (high))))
  
(corp-name-recognition2
  ;; A sequence of capitalized words before a corporate designator is 
  ;; considered to be a company name. The corporate designators and 
  ;; typical sentence-initial words are excluded from the sequence even 
  ;; if they are capitalized
  :precondition (is-mixed-case)
  (seq
   (+ (seq (word 
	    (and (reg-match "['0-9]*[A-Z].")
		 (not (in (Inc. Ltd. Corp. Univ. University Institute Labs Laboratories Laboratory A.G. Co.)))
		 (not (in begwords.txt))))
	    (? (word (in (& "&amp;" and "(" ")" - *LRB* *RRB*))))))
   (? (word (in (& , "&amp;" and))))
   (+ (lexitem (and (att (contain syn ((sem (+corpdesig)))))
		    (cap))))
   (or-pat (word (or (reg-match [^A-Z]) (in titles.txt)))
	   (end-of-sentence))
   )
  (when (not (exist-lexitem (low) ($ 3 (high)) (pred (t))))
;	     (or ($ 3 (matching-word (reg-match ".*[.]" "...?$")))
;		 (followed-by-word (pred (or (reg-match [^A-Z]) (in titles.txt))))))
    (assert (make-lexitem (low) ($ 3 (high)) (avm syn (PN (sem (+corpname))))))
    (delete-smaller ($ 1 (low)) ($ 3 (high)))))

(corp-name-recognition3
  ;; A sequence of capitalized words after typical company type, such
  ;; as "company," "manufacturer," and "maker," are considered to be 
  ;; company names. For example, "The software maker XYZ went public 
  ;; last year."

  :precondition (is-mixed-case)
  (seq 
   (word (in business.txt))
   (+ (word (reg-match [A-Z-])))
   (or-pat (word (reg-match [^A-Z]))
	   (end-of-sentence)))
  (when ($ 2 (matching-word (not (str= -))))
   (assert
    (make-lexitem 
     ($ 2 (low)) ($ 2 (high)) (avm syn (PN -plu +3sg (sem (+corpname))))))
   (delete-smaller ($ 2 (low)) ($ 2 (high)))))

(corp-name-recognition4
  ;; The names of many law firms are a sequence of person names. For
  ;; example "Blunt Ellis & Loewi." This rule covers this type of names. 
  :precondition (is-mixed-case)
  (seq
   (+ (word (and (reg-match [A-Z]) (not (in begwords.txt prons.txt)))))
   (* (seq (word (str= ,))
	   (word (reg-match [A-Z]))))
   (word (in (& "&amp;")))
   (word (reg-match [A-Z]))
   (? (lexitem (att (contain syn ((sem (+corpdesig)))))))
   (or-pat (word (reg-match [^A-Z]))
	   (end-of-sentence))
   )
  (prog
   (assert
    (make-lexitem
     (low) ($ 5 (high)) (avm syn (PN -plu +3sg (sem (+corpname))))))
   (delete-smaller (low) ($ 5 (high)))))

(corp-name-recognition5
  ;; This is a MUC-6 specific rule. In "GE's NBC unit," "NBC unit" 
  ;; should be considered a single name.
  :precondition (is-mixed-case)
  (seq
   (word (in (' 's its)))
   (lexitem (and (att (contain syn ((sem (+corpname)))))
		 (cap)))
   (word (str= unit))
   )
  (prog
   (combine-lexitems
    ($ 2 (litem))
    (make-lexitem ($ 3 (low)) ($ 3 (high)) (avm syn ()))
    )
   (delete-smaller ($ 2 (low)) (high))))

(title-and-title
  ;; Group a sequence of titles together (MUC-6 specific), e.g.,
  ;; "CEO and president"
  (seq
   (lexitem (att (contain syn ((sem (+title +post))))))
   (? (seq
       (word (str= ,))
       (lexitem (att (contain syn ((sem (+title +post))))))))
   (word (str= and))
   (lexitem (att (contain syn ((sem (+title +post)))))))
  (prog
   (combine-lexitems
    ($ 4 (litem))
    (add-components
     ($ 1 (litem))
     ($ 2 (low))
     ($ 2 (high))
     (pred (att (defined (sem))))))
   (delete-smaller (low) (high))))

(title-elect-nominate
  (seq
   (lexitem (att (contain syn ((sem (+title))))))
   (word (str= -))
   (word (in (nominate elect))))
  (prog
   (delete-smaller (low) (high))
   (assert (make-lexitem (low) (high) ($ 1 (meaning))))))

(person-name-recognition0
  ;; a sequence of capitalized words followed by Jr. Sr. ... etc. is 
  ;; a name.
;  :precondition (is-mixed-case)
  (seq 
   (? (lexitem (att (contain syn ((sem (+title)))))))
   (+ (word (and (reg-match "[']?[A-Z]" van$ von$ de$)
		 (not (suffix ("'" "'s")))
		 (not (in titles.txt))
		 (not (in begwords.txt)))))
   (? (word (str= ,)))
   (word (in (Jr. Sr. Sr Jr II III M.D. Ph.D. MS MSc)))
   (constraint (or ($ 4 (matching-word (not (in (II III)))))
		   ($ 3 (matching-word)))))
  (when (not (exist-lexitem (low) (high)
			    (pred (att (defined syn (sem))))))
    (delete-if ($ 2 (low)) (high) (pred (t)))
    (combine-lexitems
     (make-lexitem ($ 2 (low)) (high)
		   (avm syn (PN -det -plu +3sg (sem (+person)))))
     ($ 1 (litem)))
    (delete-smaller (low) (high))
    ))

(person-name-recognition1
  ;; A title followed by a sequence of capitalized words is a name.
  :precondition (is-mixed-case)
  (seq :front-to-back
   (lexitem (or (att (contain syn ((sem (+title)))))
		(root (in titles.txt))))
   (+ (word (and (reg-match "[']?[A-Z-]" O$ van$ von$ de$)
		 (not (in months.txt titles.txt orgtype.txt (I Dollars)))
		 (not (suffix ("'" "'s")))
		 ))
      (constraint ($ 1 (matching-word (reg-match "[A-Z]" van$ von$ de$))))
      )
   )
  (prog
   (if ($ 1 (litem (root (or (reg-match [A-Z]) (str= miss)))))
       (combine-lexitems
	(make-lexitem ($ 2 (low)) (high)
		      (avm syn (PN -det -plu +3sg (sem (+person))))
		      :head ($ 2 (high))
		      :root _PER)
	title ($ 1 (litem)))
     (combine-lexitems
      ($ 1 (litem))
      person (make-lexitem ($ 2 (low)) (high)
			   (avm syn (PN -det -plu +3sg (sem (+person))))
			   :head ($ 2 (high))
			   :root _PER
			   )))
   (delete-smaller (low) (high))))


(person-name-recognition2
  ;; A sequence of capitalized given name followed by a capitalized word 
  ;; is consider to be a person's name.
  :precondition (is-mixed-case)
  (seq :front-to-back
   (or-pat (word (or (reg-match [^A-Z-])
		     (in said.txt begwords.txt)))
	   (beginning-of-sentence))	   
   (lexitem (and (cap)
		 (ending bare)
		 (att (contain syn ((sem (+gname)))))))
  
   (+ (word (or (reg-match "[A-Z][.]$" "[A-Z][.][A-Z]" O$ van$ von$ de$)
		(and (reg-match "[']?[A-Z][^.]")
		     (not (in titles.txt orgtype.txt (Friday Monday Saturday Sunday Thursday Tuesday Wednesday)))))))
   (or-pat (word (reg-match [^A-Z0-9-]))
	   (end-of-sentence)))   
  (when (not (or ($ 3 (matching-word (reg-match "[A-Z][.]$" "[A-Z][.][A-Z]")))
		 (exist-lexitem ($ 2 (low)) ($ 3 (high)))))
    (set-head
     (assert (make-lexitem ($ 2 (low)) ($ 3 (high))
	     (avm syn (PN -plu +3sg (sem (+person))))))
     ($ 3 (high)))
   (delete-smaller ($ 2 (low)) ($ 3 (high)))))

(person-name-recognition3
  ;; a sequence of capitalized word followed by a family name is 
  ;; a person's name
  :precondition (is-mixed-case)
  (seq 
   (+ (word (and (or (reg-match [A-Z]) (in (van von de la)))
		 (not (suffix ("'" "'s")))
		 (not (in said.txt titles.txt begwords.txt geoentity.txt orgtype.txt)))))
   (lexitem (and (cap)
		 (ending bare)
		 (att (contain syn ((sem (+fname)))))))
   )
  (when (not (exist-lexitem (low) (high)
			    (pred (att (defined syn (sem))))))
   (delete-if (low) (high) (pred (t)))
   (assert (make-lexitem (low) (high)
           (avm syn (PN -plu +3sg (sem (+person))))))))

(person-name-recognition4
 :precondition (is-mixed-case)
 (lexitem (and (cap)
	       (ending bare)
	       (att (or (contain syn ((sem (+fname))))
			(contain syn ((sem (+gname))))))))
 (when (matching-word (not (or (in begwords.txt dates.txt)
			       (reg-match "[A-Z]+$"))))
   (if (find-prev-occ (low) (high)
		      (named-obj the-known-organizations AntecedentList))
       (litem (att (unify ((sem (+corpname))))))
     (litem (att (unify ((sem (+person)))))))))

(person-name-recognition5
 :precondition (not (is-mixed-case))
 (seq
  (+ (and-pat (or-pat (word (reg-match ".[.]"))
		      (no (lexitem (t)))
		      (lexitem (and (ending bare)
				    (att (contain syn ((sem (+gname))))))))
	      (word (reg-match "[a-zA-Z]"))))
  (lexitem (and (ending bare)
		(att (contain syn ((sem (+fname)))))))
  )
 (assert (make-lexitem (low) (high)
		       (avm syn (PN -plu +3sg (sem (+person)))))))

(person-name-recognition6
 :precondition (not (is-mixed-case))
 (seq
  (lexitem (att (contain syn ((sem (+title))))))
  (+ (and-pat (or-pat (word (reg-match "[a-zA-Z][.]"))
		      (no (lexitem (att (or (contain ((cat v)))
					    (contain ((cat i)))
					    (contain ((cat j)))
					    (contain ((cat d)))
					    (contain ((cat p))))))))
	      (word (and (reg-match [a-zA-Z])
			 (not (in titles.txt))))))
  )
 (if ($ 1 (litem (root (or (reg-match [A-Z]) (str= miss)))))
     (combine-lexitems
      (make-lexitem ($ 2 (low)) (high)
		    (avm syn (PN -det -plu +3sg (sem (+person))))
		    :head ($ 2 (high))
		    :root _PER)
      title ($ 1 (litem)))
   (combine-lexitems
    ($ 1 (litem))
    person (make-lexitem ($ 2 (low)) (high)
			 (avm syn (PN -det -plu +3sg (sem (+person))))
			 :head ($ 2 (high))
			 :root _PER
			 )))
 )

;(person-name-recognition7
; :precondition (not (is-mixed-case))
; (seq
;  (+ (and-pat (or-pat (word (reg-match ".[.]"))
;		      (no (lexitem (t)))
;		      (lexitem (and (ending bare)
;				    (att (contain syn ((sem (+gname))))))))
;	      (word (reg-match "[a-zA-Z]"))))
;  (and-pat (or-pat (no (lexitem (t)))
;		   (lexitem (and (ending bare)
;				 (att (contain syn ((sem (+fname))))))))
;	   (word (reg-match "[a-zA-Z]")))
;  )
; (when (not (exist-covering-lexitem (low) (high)))
;   (assert (make-lexitem (low) (high)
;			 (avm syn (PN -plu +3sg (sem (+person))))))))

(location-recognition
  ;; If a sequence of capitalized words followed by "," and then by
  ;; a country or province name, it must be a city name. 
;  :precondition (is-mixed-case)
  (seq 
   (+ (and-pat (word 
		(and (reg-match [A-Z&-])
		     (not (in begwords.txt (D. R. the a an both on in)))))
	       (no (lexitem 
		    (att (or (contain syn ((sem (+corpname))))
;			     (contain syn ((sem (+person))))
			     (contain syn ((sem (+country))))))))))
   (word (str= ,))
   (lexitem (and (cap)
		 (or (att (contain syn ((sem (+province)))))
		     (att (contain syn ((sem (+country))))))))
   (? (word (str= ,))))
  (when (not (and (exist-lexitem ($ 1 (low)) ($ 1 (high))
				 (pred (att (contain ((sem (+province)))))))
		  ($ 3 (litem (att (contain ((sem (+province)))))))))
    (set-high
     (combine-lexitems
      (assert (make-lexitem ($ 1 (low)) ($ 1 (high)) 
			    (avm syn (PN (sem (+city +location))))))
      inside ($ 3 (litem)))
     (high))
    ($ 3 (litem (att (unify ((sem (+location)))))))
    (delete-smaller (low) (high)))
  )

(location-recognition1
  ;; a sequence capitalized words followed by 'river', 'province', ... must
  ;; be a location.
  :precondition (is-mixed-case)
  (seq 
   (+ (word (and (reg-match [A-Z'-] &$)
		 (not (in begwords.txt (D. R. the a an both on in ' 's))))))
   (word (and (reg-match [A-Z]) (in geoentity.txt (airport)))))
  (when (and (not (exist-lexitem (low) (high)  (pred (t))))
	     ($ 1 (matching-word (not (str= -)))))
   (delete-except (low) (high) (nil))
   (set-head
    (assert (make-lexitem (low) (high) (avm syn (PN (sem (+location))))))
    ($ 2 (low)))
   ))

(location-recognition2
  :precondition (is-mixed-case)
  (seq 
   (word (in (Lake Mount Mt. River)))
   (+ (word (and (reg-match [A-Z-])
		 (not (in (river lake mount mt.))))))
   )
  (prog
   (assert (make-lexitem (low) (high) (avm syn (PN (sem (+location))))))
   (delete-smaller (low) (high))))


(set-location-attribute
  ;; cities, provinces and countries are locations.
  (lexitem (att (or (contain syn ((sem (+city))))
		    (contain syn ((sem (+province))))
		    (contain syn ((sem (+country)))))))
  (litem (att (unify ((sem (+location)))))))

(location-recognition3
  :precondition (is-mixed-case)
  (seq 
   (word (and (reg-match [A-Z])
	      (in (north south east west northwest northeast southwest southeast southern northern eastern western))))
   (lexitem (att (contain (N (sem (+location))))))
   )
  (prog
   (assert (make-lexitem (low) (high) (avm syn (PN (sem (+location))))))
   (delete-smaller (low) (high))))


(address2
 (seq (word (reg-match [0-9]))
      (* (word (reg-match [A-Z] "[0-9]+((th)|(st)|(nd)|(rd))")))
      (* (word (reg-match "[#]" "Apt" [0-9,])))
      (lexitem (att (contain ((sem(+city))))))
      ) 
 (prog
  (combine-lexitems
   (assert (make-lexitem (low) ($ 3 (high)) (avm syn (PN (sem (+address))))))
   city ($ 4 (litem)))
  (delete-smaller (low) (high))
  ))

(address1
 (seq (+ (word (and (reg-match [0-9A-Z-] "[0-9]+((th)|(st)|(nd)|(rd))")
		    (not (in (which what a))))))
      (word (comp (reg-sub "^[0-9]*" "") (in address.txt)))
      (* (word (and (reg-match "[#]" "Apt" [0-9,])
		    (not (comp (reg-sub "^[0-9]*" "") (in address.txt))))))
      (? (word (in (north south east west N.E. N.W. S.E. S.W. N.E N.W S.E S.W))))
      (? (lexitem (att (contain ((sem(+city)))))))
      ) 
 (prog
  (if ($ 5 (litem))
      (combine-lexitems
       (assert (make-lexitem (low) ($ 4 (high)) (avm syn (PN (sem (+address))))))
       city ($ 5 (litem)))
    (assert (make-lexitem (low) (high) (avm syn (PN (sem (+address)))))))
  (delete-smaller (low) (high))
  ))

;(debug :act-on-all-matches (lexitem (t)) (nil))

(phone-number
 (seq (? (word (str= "(")))
      (? (word (reg-match "[0-9][0-9][0-9]$")))
      (? (word (or (str= -) (str= ")"))))
      (word (reg-match "[0-9][0-9][0-9]$"))
      (? (word (str= -)))
      (word (reg-match "[0-9][0-9][0-9][0-9]$"))
      )
 (prog
  (delete-smaller (low) (high))
  (assert (make-lexitem (low) (high) (avm syn (PN (sem (+phone))))))
  ))

;(adj-in-ed-or-ing-form
;  ;; -ed or -ing verbs can be used as adjectives only if they are not
;  ;; preceded by a form of 'be' or 'have'
;  (seq :front-to-back
;   (word (in (is are am been was were be have has had 't)))
;   (* (lexitem (att (contain (Adv)))))
;   (or-pat (word (and (suffix (ed ing))
;		      (not (in (
;alarming annoying appalling astonishing astounding chilling devastating
;discouraging disgusting distressing frightening horrifying irritating puzzling
;shocking startling terrifying troubling upsetting declining 
;)))))
;	   (lexitem (att (contain ((vform ed))))))
;   )
;  (prog
;   (delete-if ($ 3 (low)) (high) (pred (att (contain syn ((cat a) -prd)))))
;   (delete-if (low) ($ 1 (high)) (pred (att (not (contain syn ((cat v)))))))
;   )
;  )

(verb-as-adj
 ;; "an recently enacted law"
  (seq
   (lexitem (att (contain ((cat d)))))
   (lexitem (att (contain ((cat a)))))
   (lexitem (and (ending -ed)
		 (att (contain (Tn))))))
  (assert (make-lexitem ($ 3 (low)) ($ 3 (high)) (avm syn (Aatt))))
  )

(llrule
 (lexitem (lexitem-meaning (meaning-type (str= llrule))))
 (prog
  (apply-llrule)
  (delete (litem))
  ))

;(verb-as-adj
;  ;; -ed or -ing verbs can be used as adjectives if they are not
;  ;; preceded by a form of 'be' or 'have'
;  (seq
;   (or-pat
;    (word (not (in (is are am been was were be have has had 't))))
;    (beginning-of-sentence))
;   (and-pat (word (t))
;      (lexitem
;       (and
;	(att (contain syn ((cat v))))
;	(or (and
;	     (att (contain syn ((args ((cat n))))))
;	     (or (ending -ed)
;		 (att (contain syn ((vform ed))))))
;	    (and
;	     (ending -ing)
;	     (no-alternative (att (contain syn (N)))))
;;	    (or (att (contain syn ((args (+opt)))))
;;		(att (unifiable (~args)))))
;;	    (modifier (and (defined syn (ref))
;;			   (unifiable syn ((agr (~tense))))))
;	    )
;	(root (not (in (have say))))
;	(no-alternative (att (contain syn (A))))))))
;  (if ($ 2 (matching-word (reg-match .*ing)))
;      (set-weight 
;       (set-head (assert (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn (N -prd +att))))
;		 ($ 2 (low)))
;       (int 20))
;    (set-weight 
;     (set-head (assert (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn (Aatt))))
;	       ($ 2 (low)))
;     (int 20)))
;  )


;(ref
;  (lexitem (and (modifier (not (nil)))
;		(not (ending bare))))
;  (delete (litem)))

(document-title
 (seq (word (in ("\"" "'" <ldquo>)))
      (+ (word (and (not (in ("\"" *LRB* *RRB* <ldquo> <rdquo>)))
		    (or (reg-match [A-Z])
			(in prep.txt (the a an -))))))
      (word (in ("\"" "'" <rdquo> *RRB*))))
 (when ($ 2 (matching-word (reg-match "[A-Z]")))
  (delete-smaller (low) (high))
  (assert (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn (PN))))
  ))

;(quote
;  (seq (word (in ("\"" <ldquo>)))
;       (+ (word (not (in ("\"" *LRB* *RRB* <ldquo> <rdquo>)))))
;       (word (in ("\"" <rdquo> *RRB*))))
;  (prog
;   (isolate ($ 2 (low)) ($ 2 (high)))
;   (skip ($ 1 (low)))
;   (skip ($ 3 (low)))))

;(quoted-cap-words
;  :precondition (is-mixed-case)
; (seq (word (in (<dquote> <lsquo> <ldquo> *LRB*)))
;      (? (word (reg-match [A-Z0-9])))
;      (? (word (reg-match [A-Z0-9])))
;      (word (reg-match [A-Z0-9]))
;      (word (in (<dquote> <rdquo> <rsquo> *RRB*))))
; (assert (make-lexitem ($ 2 (low)) ($ 4 (high)) (avm syn (N)))))


(cap-word-sequence
  ;; Determine the type of a sequence of capitalized words
  :precondition (is-mixed-case)
  (seq (* (or-pat
	   (word (and (reg-match "[A-Z]") 
		      (not (in begwords.txt said.txt titles.txt months.txt dates.txt))))
	   (seq (word (reg-match "[A-Z]"))
		(word (reg-match "[&-]$" "&amp;")))))
       (word (and (reg-match [']?[A-Z].) 
		  (not (in begwords.txt prons.txt titles.txt months.txt dates.txt))))
       (? (word (reg-match [0-9])))
       (or-pat (word (reg-match [^A-Z0-9-]))
	       (end-of-sentence))
       )
  (when (not (exist-lexitem
	      (low) ($ 2 (high)) 
	      (pred 
	       (att 
		(value 
		 sem "AttVec*"
		 (defined-one-of sem 
		   (money time date corpname percent location person title)))))))
    (let (new-item)
      (cond
;       ((setq new-item
;	      (find-prev-occ (low) ($ 2 (high))
;			     (named-obj the-known-locations AntecedentList)))
;	(delete-except (low) ($ 2 (high)) (get-val new-item))
;	)
       ((setq new-item
	      (find-prev-occ (low) ($ 3 (high))
			     (named-obj the-known-organizations AntecedentList)))
	(delete-except (low) ($ 3 (high)) (get-val new-item))
	)
       ((setq new-item
	      (find-prev-occ (low) ($ 3 (high))
			     (named-obj the-known-persons AntecedentList)))
	(delete-except (low) ($ 3 (high)) (get-val new-item))
	)
;     ((and (exist-word-in-sentence (low) ($ 3 (high)) (pred (in geoentity.txt)))
;	   (setq new-item (make-lexitem (low) ($ 3 (high)) (avm syn (PN (sem (+location)))))))
;      (delete-except (low) ($ 3 (high)) (get-val new-item))
;      (assert (get-val new-item))
;      )
       ((and (not (exist-covering-lexitem (low) ($ 3 (high)) (pred (t))))
	     (not (exist-overlapping-lexitem (low) ($ 3 (high)) (pred (t)))))
	(setq new-item (make-lexitem (low) ($ 3 (high)) (avm syn (N))))
	(cond
	 (($ 3 (matching-word (in (line series))))
	  (get-val new-item (att (unify ((sem (+product))))))
	  )
	 (($ 3 (matching-word (in (unit subsidiary))))
	  (get-val new-item (att (unify ((sem (+corpname))))))
	  )
;	 ((t)
;	  (get-val new-item
;		   (if (root (reg-match "[A-Z].*[0-9]" 
;					".*[0-9][A-Z]" 
;					".+[A-Z][0-9]"))
;		       (att (unify ((sem (+product))))))))
	 )
	(set-head (get-val new-item) ($ 3 (high)))
;	(add-components
;	 (get-val new-item)
;	 (low)
;	 ($ 3 (high))
;	 (pred
;	  (att 
;	   (value 
;	    sem "AttVec*"
;	    (defined-one-of sem 
;	      (money time date corpname percent location city province person title country))))))
	(assert (get-val new-item))
	(delete-if (low) ($ 3 (high)) (pred (not (att (contain (N))))))
	(isolate (low) ($ 3 (high)))
	)))))

(corp-name-recognition-6
  :precondition (is-mixed-case)
 (seq (* (word (and (reg-match [A-Z]) (not (in begwords.txt)))))
      (word (and (in orgtype.txt)
		 (reg-match [A-Z])))
      (word (in (of on for)))
      (lexitem (and (cap)
		    (att (unifiable syn (N -adv))))))
 (prog
  (assert
   (make-lexitem
    (low) (high) (avm syn (PN -plu +3sg (sem (+corpname))))))
  (delete-smaller (low) (high))))

(corp-name-recognition7
 :precondition (not (is-mixed-case))
 (seq (or-pat (word (in before-org.txt))
	      (beginning-of-sentence))
      (* (word (and (not (in before-org.txt))
		    (reg-match [a-zA-Z&-])
		    (not (suffix (ed ly ing))))))
      (word (not (in before-org.txt)))
      (word (in capdesig.txt)))
 (when (not (exist-covering-lexitem ($ 2 (low)) (high)))
   (assert
    (make-lexitem
     ($ 2 (low)) (high) (avm syn (PN -plu +3sg (sem (+corpname)))))))
 )

(corp-name-recognition8
 :precondition (not (is-mixed-case))
 (seq (? (word (str= the)))
      (+ (and-pat (word (and (reg-match [a-zA-Z&-])
			     (not (in before-org.txt))))
		  (no (lexitem (att (or (contain ((cat v)))
					(contain (+pron))))))))
      (word (in orgtype.txt)))
 (when (not (exist-covering-lexitem ($ 2 (low)) (high)))
   (assert
    (make-lexitem
     ($ 2 (low)) (high) (avm syn (PN -plu +3sg (sem (+corpname)))))))
 )

(corp-name-recognition9
 :precondition (not (is-mixed-case))
 (+ (word (covered-by-antecedent the-known-organizations)))
 (when (not (exist-lexitem (low) (high)
			   (pred (att (and (contain syn (N))
					   (defined (sem)))))))
   (assert
    (make-lexitem
     (low) (high) (avm syn (PN -plu +3sg (sem (+corpname))))))))

(corp-name-recognition10
 :precondition (not (is-mixed-case))
 (word (and (reg-match ...)
	    (is-abbrev the-known-organizations)))
 (when (not (exist-lexitem (low) (high)
			   (pred (att (contain syn (N (sem (+corpname))))))))
   (delete-if (low) (high) (pred (t)))
   (assert
    (make-lexitem
     (low) (high) (avm syn (PN -plu +3sg (sem (+corpname))))))))

(corp-location
  ;; company name follow by a location in parentheses.
  (seq
   (lexitem (att (contain syn ((sem (+corpname))))))
   (word (in ("(" *LRB* ,)))
   (lexitem (att (contain syn ((sem (+location))))))
   (word (in (")" *RRB* ,)))
   )
  (prog
   (set-high
    (combine-lexitems
     ($ 1 (litem))
     location ($ 3 (litem))
     )
    (high)
    )
   (delete-smaller (low) (high))))

(corp-location-2
  ;; company name follow by a location in parentheses.
  (seq
   (lexitem (att (contain syn ((sem (+corpname))))))
   (word (str= of))
   (lexitem (att (contain syn ((sem (+location))))))
   )
  (combine-lexitems  ($ 1 (litem))  location ($ 2 (litem)))
  )

;;(corp-location-3
;;  ;; LOCATION-based XXX
;;  (seq
;;   (or-pat (lexitem (att (contain syn ((sem (+location))))))
;;	   (+ (word (reg-match [A-Z]))))
;;   (? (word (str= -)))
;;   (word (str= based))
;;   ;; MISSING SOMETHING HERE
;;   )
;;  (prog
;;   (combine-lexitems
;;    (if ($ 1 (litem))
;;	($ 1 (litem))
;;      (make-lexitem (low) ($ 1 (high)) (avm syn (PN (sem (+location)))))
;;      )
;;    (make-lexitem ($ 3 (low)) ($ 3 (high)) (avm syn ())))
;;   (delete-smaller (low) (high))))

(corp-location-4
  (seq
   (+ (word (and (reg-match "[A-Z].")
		 (not (in begwords.txt)))))
   (word (in ("(" *LRB*)))
   (lexitem (att (contain syn ((sem (+location))))))
   (word (in (")" *RRB*)))
   )
  (prog
   (set-high
    (combine-lexitems
     (make-lexitem (low) ($ 1 (high)) (avm syn (PN (sem (+corpname)))))
     location ($ 3 (litem))
     )
    (high)
    )
   (delete-smaller (low) (high))))

(unknown-word
   ;; Guess the category of an unknown word
  (not-in-lexicon)
  (when (not (or (matching-word
		  (or (reg-match "[,!.:;\"'`()@?{}$=_*<>&|\\-]+$" "\\[$" "\\]$")
		      (in ("&mdash;" "&ndash;" "&quot;" "&ldquo;" "&rdquo;" "&UR;" "&LR;" "&QL;" "&QR;" "&QC;" "&HT;"))))
		 (exist-covering-lexitem (low) (high))))
    (assert
     (cond 
      ((matching-word (reg-match <))
       (if (matching-word (or (str= <formul>) (reg-match "<ANS")))
	   (make-lexitem (low) (high) (avm syn (N)))
	 (nil)
	 ))
      ((matching-word (in (*LRB* *RRB* "(" ")")))
       (nil))
      ((matching-word (reg-match "[A-Z][A-Z0-9\$]"))
       (make-lexitem (low) (high) (avm syn ((cat n) +pn (nform norm))))
       )
      ((matching-word (reg-match "[0-9.$=*@]"))
       (make-lexitem (low) (high) (avm syn (N)))
       )
      ((and (preceded-by-word (pred (str= to)))
	    (followed-by-word (pred (in (the an a some many)))))
       (make-lexitem (low) (high) (avm syn (Tn (vform bare)))))
      ((matching-word (or (suffix (wide like sive ble ed ing al ic ive lar cent ous less tory ary))
			  (reg-match extra un anti)))
       (make-lexitem (low) (high) (avm syn (Adj)))
       )
      ((matching-word (suffix (ly)))
       (make-lexitem (low) (high) (avm syn (Adv)))
       )
      ((t)
       (make-lexitem (low) (high) (avm syn (N)))
       )))
    ))

(list-bullets-are-not-nouns
 (seq (beginning-of-sentence) 
      (word (str= o)))
 (delete-if (low) (high) (pred (t))))

(re-
 (seq (word (str= re))
      (word (str= -))
      (lexitem (t)))
 (set-low ($ 3 (litem)) (low))
 )

(hyphen-unknown1
  ;; guess the POS of a hyphenated word
  (seq 
   (+ (seq
       (or-pat (lexitem (att (not (or (contain (Adv))
				      (contain ((cat i)))))))
	       (word (reg-match [A-Za-z0-9])))
       (word :nogap (or (str= -) (str= "&ndash;")))
       (? (word (in (, and or))))))
   (word :nogap (t)) ;(not (in (a the an - and or , have had) skip.txt auxiliary.txt prons.txt)))
   )
  (hyphen-unknown-func)
  )

;(hyphen-unknown2
;  (seq 
;   (word (not (str= -)))
;   (word (str= -))
;   (lexitem (t)))
;  (when (not (exist-lexitem (low) (high)))
;    (set-low ($ 3 (litem)) (low))))

(possessive1
  ;; If the dictionary contains a word in possessive form, then discard the
  ;; word without the possessive affix.
  (seq (word (t))
       (word (in ('s 'S ')))
       (constraint
	(or ($ 2 (matching-word (not (str= '))))
	    ($ 1 (matching-word (reg-match ".*[sS]$")))))
       )
  (prog
   (look-up (low) (high))
   (when (exist-lexitem (low) (high))
     (delete-smaller (low) (high)))))

(possessive2
  ;; If an noun is followed by 's, then create the genitive form of the noun.
  (seq (lexitem (and (att (contain syn ((nform norm) (cat n))))
		     (not (root (or (in prons.txt)
				    (in (that there who what which whoever)))))))
       (word (in ('s 'S ')))
       (constraint
	(or ($ 2 (matching-word (not (str= '))))
	    ($ 1 (matching-word (reg-match ".*[sS]$")))))
       )
  (when (not (exist-lexitem (low) (high)))
    (comp  
     (combine-lexitems
      ($ 1 (litem))
      poss (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn ())))
     (att (unify (+genitive))))
;    (delete-smaller (low) ($ 2 (high)))
    ))

(type-X
  (seq (word (in (alpha beta type class factor stage phase grade group level layer subunit site region category subgroup angiotensin topoisomerase kinase polymerase annexin anhydrase apoA metarhodopsin Secretogranin CA)))
       (word (reg-match [A-Z0-9]$ [IVX]+)))
  (prog
   (assert (make-lexitem (low) (high) (avm syn (N))))
   (delete-smaller (low) (high))
   )
  )

(x-years-old
  (seq (lexitem (root (reg-match "year")))
       (word (str= old)))
  (prog
   (assert (make-lexitem (low) (high) (avm syn (Adj))))
   (delete-smaller (low) (high)))
  )

(person-x-years-old
  (seq (lexitem (att (contain syn ((sem (+person))))))
       (word (str= ,))
       (word (reg-match [0-9][0-9]?$))
       (? (word (str= years)))
       (? (word (str= old)))
       (word (in (, .))))
  (prog
   (set-high
    (combine-lexitems
     ($ 1 (litem))
     age (make-lexitem ($ 3 (low)) ($ 5 (high)) (avm syn ()))
     )
    (high))
   (delete-smaller (low) (high))))

(person-x-self
  (seq (lexitem (att (contain syn ((sem (+person))))))
       (word (or (str= himself) (str= herself)))
       )
  (prog
    (combine-lexitems
     ($ 1 (litem))
     self (make-lexitem ($ 2 (low)) (high) (avm syn (N +pron (sem (+person)))))
     )
    (delete-smaller (low) (high))))

(skip
  ;; Allow the parser to skip these words.
  (word (in skip.txt))
  (skip (low)))

(x-of=>x-is-noun
  ;; if a word x is followed by of and x is not currently known as
  ;; a noun or conjunction, then treat x as a noun.
 (seq
  (& (word (and (reg-match [a-z]) (not (in (out) said.txt))))
     (no (lexitem 
	  (att (or (contain syn (N))
		   (contain syn ((cat p)))
		   (contain syn ((cat i)))
		   (contain syn ((cat a)))
		   (contain syn ((cat j))))))))
  (word (str= of)))
 (when (not (exist-lexitem (low) (high)))
   (assert (make-lexitem (low) ($ 1 (high)) (avm syn (N))))))

(rate
  ;; recognize rate, e.g., "$4 per hour", "$120 per share".
  (seq (lexitem (att (or (contain syn ((sem (+number))))
			 (contain syn ((sem (+money)))))))
       (word (in (a an per each)))
       (lexitem (root (in units.txt))))
  (prog
   (delete-if (low) (high) (pred (t)))
   (assert (make-lexitem (low) (high) (comp ($ 1) (meaning))))))

(del-have
  ;; if have/has is followed by -ed or -en verb, it is not a verb
  (seq (lexitem (and (root (str= have))
		     (att (contain syn ((cat v))))))
       (word (and (not (reg-match un))
		  (not (in (seven been)))
		  (suffix (ed en)))))
  (delete ($ 1 (litem))))

(the-adj-as-noun
  ;; if an adjective is preceded by the and followed by something that 
  ;; is not a noun or adjective, then the adjective is treated as a noun
  (seq (word (in (the a an)))
       (lexitem (and (att (and (contain syn ((cat a)))
			       (unifiable syn (-adv))))
		     (no-alternative (att (contain syn (N)))))))
  (when (not (or (followed-by (pred (or (ending -ing)
					(att (or (contain syn ((cat a) -adv))
						 (contain syn ((cat n))))))))
		 (followed-by-word (pred (in (and or ,) skip.txt)))))
    (assert (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn (N))))))

(verb-as-adj
  ;; if an adjective is preceded by the and followed by something that 
  ;; is not a noun or adjective, then the adjective is treated as a noun
  (seq (word (in (the a an) prep.txt))
       (lexitem (and (att (contain syn (verb (vform ed))))
		     (no-alternative (att (contain syn (A)))))))
  (assert (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn (Aatt)))))

(said-1
  (seq (word (in (, ? ! .)))
       (* (word (in skip.txt)))
       (word (in said.txt)))
  (prog
   (assert
    (make-lexitem
     ($ 3 (low)) ($ 3 (high)) (avm syn (verb (args (N (role subj))) (node SaidX)))))
   (when (or ($ 1 (matching-word (in (? ! .))))
	     ($ 2 (matching-word (reg-match ".*[\"']"))))
     ($ 3 (keep-tags (pred (att (contain ((node SaidX))))))))))
     

(said-2
 (seq (word (in said.txt))
      (* (word (in skip.txt)))
      (or-pat (word (in (, . ? ! )))
	      (lexitem (att (contain (Nadv))))
	      (end-of-sentence)))
 (assert
  (make-lexitem
   ($ 1 (low)) ($ 1 (high)) (avm syn (verb (args (N (role subj))) (node XSaid) +head_final)))))

(aux+be
 (seq (word (in (can will to)))
      (word (str= be))
      )
 (delete-if ($ 1 (low)) ($ 1 (high)) (pred (att (not (defined (auxform)))))))

;(to-verb ;; tagging rule
;  ;; if 'to' is followed by a verb without rare attribute, then treat
;  ;; 'to' as infinitive marker and delete other meanings of 'to' and
;  ;; the verb.
;  (seq :front-to-back
;   (word (str= to))
;   (lexitem (and (att (unifiable syn ((cat v) ~rare)))
;		 (ending bare))))
;  (prog
;   (delete-if ($ 1 (low)) ($ 1 (high))
;	      (pred (not (att (unifiable ((cat i)))))))
;   (delete-if ($ 2 (low)) ($ 2 (high))
;	      (pred (not (or (att (unifiable ((cat v))))
;			     (att (unifiable ((cat i))))))))))	      

(own
 (seq (lexitem (att (contain syn (+genitive))))
      (word (str= own)))
 (prog
  (delete-smaller (low) (high))
  (set-high ($ 1 (litem)) (high))))

; this rule should make sure that the verbs have the same ending
(conj-Tn
 (seq (lexitem (att (contain syn (Tn))))
      (* (seq (word (str= ,))
	      (lexitem (att (contain syn (Tn))))))
      (? (word (str= ,)))
      (word (in (and or)))
      (lexitem (att (contain syn (Tn)))))
 (when (consistent-components)
   (add-components ($ 1 (litem)) (low) (high) (pred (att (contain syn (Tn)))))
   )
 )

(conj-Tfw
 (seq (lexitem (att (contain syn (Tfwg))))
      (* (seq (word (str= ,))
	      (lexitem (att (contain syn (Tfwg))))))
      (? (word (str= ,)))
      (word (in (and or)))
      (lexitem (att (contain syn (Tfwg)))))
 (add-components ($ 1 (litem)) (low) (high) (pred (att (contain syn (Tfwg)))))
 )

(colon-at-end-of-sentence
 (seq (no (lexitem (att (contain syn (N)))))
      (word (str= :))
      (end-of-sentence))
 (assert (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn (N)))))


(plural-in-paranthese
  (seq (lexitem (att (contain (N))))
       (word (str= "("))
       (word (str= s))
       (word (str= ")")))
  (prog
   (set-high ($ 1 (litem)) (high))
   ($ 1 (litem (att (unify (+plu))))))
  )


(prep-followed-by-verb
 (lexitem (and (att (contain ((cat p))))
	       (root (not (in (to but before past as "on to"))))))
 (del-following
  (pred
   (att (and (contain (verb))
	     (not (contain ((vform ing))))
	     )))))
	     
(auxilary
 (lexitem (att (and (defined (auxform))
		    (not (contain ((auxform to))))
		    (not (contain ((auxform do))))
		    (not (contain ((auxform need)))))))
 (when (followed-by (pred (and (ending bare)
			       (att (contain ((cat v)))))))
   (delete-if (low) (high) (pred (not (att (defined (auxform))))))))

(auxilary-to
 (lexitem (att (contain ((cat p) (pform to)))))
 (when (and (followed-by (pred (and (ending bare)
				    (att (contain ((cat v))))))))
   (add-weight (litem) (int 15))))

(adj-modifier-of-proper-noun
 (seq (lexitem (and (att (contain (Adj)))
		    (not (or (root (reg-match .*[-]))
			     (root (in (troubled ruling owned defunct)))
			     (no-alternative (att (unifiable syn (A))))))))
      (lexitem (att (value 
		     sem "AttVec*"
		     (defined-one-of sem 
		       (money time date corpname percent))))))
 (set-weight ($ 1 (litem)) (int 10)))

(compound-noun
  ;; A compound noun has the attribute value +cn then it is most
  ;; likely used as a compound noun whenever they appear together
  ;; in a sequence. Therefore, delete all other lexical items for
  ;; each of the individual words. For example, since "interest rates"
  ;; has the +cn attribute, do not treat "interest" and "rate" as
  ;; individual nouns or verbs.
  :act-on-all-matches
  (lexitem (att (contain syn (+cn))))
  (when (not (exist-overlapping-lexitem (low) (high) (pred (t))))
    (delete-if (low) (high) (pred (att (not (unifiable ((cat n)))))))
    (isolate (low) (high))))
;    (delete-smaller (low) (high))))



(well-known-noun-adjective-modification
  :act-on-all-matches
  (seq :overlapping
   (lexitem (att (contain syn (A))))
   (lexitem (att (contain syn (N))))
   )
  (when (and ($ 1 (litem (not (is-deleted))))
	     ($ 2 (litem (not (is-deleted))))
	     (comp
	      (format ($ 1 (litem (root))) "|" ($ 2 (litem (root))))
	      (in :wordlist adj-noun)))
    (delete-if (low) ($ 1 (high)) (pred (att (not (unifiable (A))))))
    (delete-if ($ 2 (low)) (high) (pred (att (not (unifiable ((cat n)))))))
    ))


(well-known-noun-noun-modification
  :act-on-all-matches
  (seq :overlapping
   (lexitem (and (att (contain syn (N)))
		 (ending bare)))
   (lexitem (att (contain syn (N))))
   )
  (when (comp
	 (format ($ 1 (litem (root))) "|" ($ 2 (litem (root))))
	 (in :wordlist nn))
;    (comp (format ($ 1 (litem (root))) "|" ($ 2 (litem (root)))) (print-str))
    (delete-if (low) ($ 1 (high)) (pred (att (not (unifiable (N))))))
    (delete-if ($ 2 (low)) (high) (pred (att (not (unifiable (N))))))
    ))

(phrasal-entry
  :act-on-all-matches
  (lexitem (att (contain syn (+phrase))))
  (when (not (exist-overlapping-lexitem (low) (high) (pred (t))))
    (delete-smaller (low) (high))))

(rare-lexitems
  (lexitem (att (defined (rare))))
  (if (litem (att (contain ((rare very)))))
      (add-weight (litem) (int 10))
    (add-weight (litem) (int 20))))

(else
 (seq
  (lexitem (root (in (anybody anyone anything everybody everyone everything somebody someone something))))
  (lexitem (and (root (str= else))
		(att (contain ((cat a))))))
  )
 (prog
  (combine-lexitems ($ 1 (litem)) else ($ 2 (litem)))
  (delete ($ 2 (litem)))
  ))

(last
 (seq
  (lexitem (and (att (contain ((cat a))))
		(root (str= last))))
  (lexitem (att (contain ((sem (+date))))))
  )
 (prog
  (combine-lexitems ($ 2 (litem)) last ($ 1 (litem)))
  (delete ($ 1 (litem)))
  ))

(set-flags :act-on-all-matches
 (lexitem (t))
 (prog
  (cond ((litem (att (and (contain syn ((args ((cat c)))))
			  (unifiable syn ((args (-bare)))))))
	 (litem (att (unify syn ((args (-bare)))))))
	((litem (att (and (contain syn ((args () ((cat c)))))
			  (unifiable syn () ((args (-bare)))))))
	 (litem (att (unify syn ((args () (-bare)))))))
	)	
  (cond ((litem (att (unifiable syn ((cat a) +adv -wh ~node))))
	 (flag-adv (litem))
	 )
	((litem (att (contain syn ((cat n) +adv))))
; 	 (if (litem (ending -s))
;	     (litem (att (modify (-adv))))
	   (flag-adv (litem))
;	   )
	)
       ((litem (att (contain syn ((cat j) +last_conj))))
	(flag-last-conjunction (litem))
	)
       ((litem (att (contain syn ((cat j)))))
	(flag-first-conjunction (litem))
	))
;;  (comp 
;;   (litem)
;;   (prog
;;    (when (att (and (not (defined (node)));; to prevent lexitems with (node NN)
;;		    ;; from being registered
;;		    (value sem "AttVec*" (defined-one-of sem (corpname person location)))))
;;      (cond
;;       ((att (contain syn ((sem (+location)))))
;;	(register-name the-known-locations))
;;       ((att (contain syn ((sem (+person)))))
;;	(register-name the-known-persons))
;;       ((att (contain syn ((sem (+corpname)))))
;;	(register-name the-known-organizations))
;;       ))
;;    (set-head-of-lexitem)
;;    ))
;;  ))
  ))

;;(nn 
;; (seq (lexitem (att (contain (N))))
;;      (* (lexitem (att (contain (N)))))
;;      (lexitem (att (contain (N)))))
;; (t)
;; )



)

(make-llrule aux-is-followed-by-verb
 (~ (lexitem (att (contain ((cat v))))))
  (prog
   ($ 1 (keep-tags (pred (att (defined (auxform))))))
   ($ 2 (keep-tags (pred (att (or (unifiable ((cat v) -be -have))
				  (contain (+be))
				  (contain (+have)))))))
   ))

(make-llrule author-of-book
  (~ (word (str= of)) (word (str= "\"")) (+ (word (not (str= "\"")))) (word (str= "\"")))
  (prog
   ($ 1 (keep-tags (pred (att (contain (N))))))
   (delete-smaller ($ 3 (low)) ($ 5 (high)))
   (assert (make-lexitem ($ 4 (low)) ($ 4 (high)) (avm syn (PN (sem (+document))))))))

(make-llrule help+verb
  (~ (lexitem (att (contain ((cat v) (vform bare))))))
  (assert
   (make-lexitem ($ 2 (low)) ($ 2 (low (-1)))
		 (avm syn ((auxform to) (cform inf) ~tense -govern (node Aux))))))

(make-llrule term-content1
  (~ (word (str= "\"")) (+ (word (not (reg-match "[.\";]")))) (word (reg-match "[.\";]")))
  (prog
   (combine-lexitems
    (retrieve-lexitem ($ 1 (low)) ($ 1 (high)) (avm syn (N)))
    content (make-lexitem ($ 3 (low)) ($ 3 (high)) (avm syn (N)))
    )
   (delete-smaller ($ 2 (low)) ($ 4 (high))))
 )

(make-llrule term-content2
  (~ (+ (word (reg-match "[A-Z]"))))
  (prog
   (combine-lexitems
    (retrieve-lexitem ($ 1 (low)) ($ 1 (high)) (avm syn (N)))
    content (make-lexitem ($ 2 (low)) ($ 2 (high)) (avm syn (N)))
    )
   (delete-smaller ($ 2 (low)) ($ 2 (high))))
 )

(make-llrule book-title
  (~ (word (reg-match "[\"']")) (word (reg-match [A-Z0-9])) (* (word (not (reg-match "[.\"';]")))) (word (reg-match "[.\"';]")))
  (prog
   (combine-lexitems
    (retrieve-lexitem ($ 1 (low)) ($ 1 (high)) (avm syn (N)))
    title (make-lexitem ($ 3 (low)) ($ 4 (high)) (avm syn (N)))
    )
   (delete-smaller ($ 2 (low)) ($ 5 (high))))
 )

(make-llrule noun-to-do
  (~ (word (str= to)) (lexitem (att (contain ((cat v) (vform bare))))))
  (prog
   ($ 2 (keep-tags (pred (att (contain ((auxform to)))))))
   ($ 3 (keep-tags (pred (att (contain ((cat v)))))))
   )
  )

