;; define the attribute value vector for the parser.
;; initialize the C++ variables that stores the index of attributes in
;; the vector as well as the values of the attributes.
(load gbatt.lsp)   ;; these two lines should be 
;; define additional attributes, this must be done before the creation
;; of any AttVec objects.

;; Additional attribute may be defined. However, this must be done before
;; the attribute value vector is used.
;;(def-att-list sem
;;  (defattribute poly BinAtt)   ;; polysemous
;;  (defattribute sense SynsetAtt) ;; the selected sense
;;  (defattribute first SynsetAtt) ;; the selected sense
;;  (defattribute referent EntAtt) ;; the referent
;;  (defattribute score ScoreAtt)    ;; the score for the chosen sense
;;  )

;; Minipar uses a set of global variables to denote the attributes
;; defined in gbatt.lsp. So the access to these attribute values can be
;; as fast as possible. This command set the values of these global variables
(init-attributes)          ;; placed before everything else

;; sets the default type of attribute value vector to be "syn"
(set-att-itp-list syn)

;;(make-avmeaning-proto <type-of-attribute-vector> <type-of-meaning>)
(make-avmeaning-proto syn syn) 

;; gblex.lsp contains the aliases of attribute value vectors.
(load gblex.lsp)

;; The following are a set of attributes in dependency trees
(define-attribute ante DepTreeValue) ;; antecedent
(define-attribute ref  DepTreeValue) ;; reference (inverse of antecedent) 
(define-attribute type StrValue)     ;; proper name classification
(define-attribute context StrValue)  ;; dependency countext
(define-attribute csw  StrValue)     ;; contextually similar words

;; create a lexicon object and name it English-lexicon
(make-lexicon 
 English-lexicon ;; name of the lexicon
 39997           ;; size of the internal hashtable
 :ending-table   ;; the types of word endings tried when looking up a word
 ((plain        ""      bare)
  (double	ing	-ing)
  (double	er	-er)
  (double	est	-est)
  (double	ier	-er)
  (double	iest	-est)
  (double	ed	-ed)
  (plain	ies	-s	y)
  (plain	ied	-ed	y)
  (plain	ier	-er	y)
  (plain	iest	-est	y)
  (plain	ves	-s	fe)
  (plain	ves	-s	f)
  (plain	er	-er	e)
  (plain	ed	-ed	e)
  (plain	est	-est	e)
  (plain	ing	-ing	e)
  (es	        es	-s	"" shxzo)
  (plain	s	-s)
  (plain	ed	-ed)
  (plain	er	-er)	
  (plain	est	-est)
  (plain	ing	-ing)
;  (plain	.	-dot)
;  (plain	:	-dot)
  )
 )

(link-lex-file English-lexicon dict.hdr)    ;; the main lexicon file
(link-lex-file English-lexicon llrule.hdr)  ;; the lexical rules triggered by words
(link-lex-file English-lexicon muclex1.hdr) ;; Some proper names form MUC data

(defmacro pitem-lexitem (exp)
  (pitem-attvec (value lexitem "LexItem*" exp)))

;; An antecedent-list contains a list of named entities recognized
;; during parsing. It can be used to recognize more named
;; entities. For example, an article may use the name "Acme, Inc." for
;; the first occurrence and "Acme" for the remaining occurrences. Once the
;; parser determines that "Acme, Inc." is a company and creates a corresponding
;; entry in the-known-organizations list, it will be able to classify
;; later instances of Acme as an organization.

(make-antecedent-list the-known-persons) 
(make-antecedent-list the-known-organizations) 
(make-antecedent-list the-known-locations) 

;; load the lexical rules
(load lexrules.lsp)

(load-lex English-lexicon manual.lsp)

;; create an lexical analyzer and name it English-lexical-analyzer
(make-lex-analyzer English-lexical-analyzer
  :suggested-sentence-length 64  ;; 
  :lexicon English-lexicon       ;; The lexicon used in lookups
  :lexical-rules English-lexical-rules  ;; run these rules after looking up wors

  ;; Action to be performed on all lexical items
  :hook (lexitemlist-hook)  

  ;; selector for types of meanings to include in lexial items
  :meaning-selector (meaning-type (or (str= avm) (str= llrule)))
  )

(def-minimalist-parser
  ;; name of the parser
  English-parser  

  ;; size of the hashtable for storing nodes
  :hashtable-size 179

  :attribute-vector-type syn

  ;; lexical attributes are only not used by the parser.
  :lexical-attributes (rare cap phrase cn allcap cmp)

  ;; By default these attributes percolate during MERGE. Some what like the
  ;; foot features in GPSG.
  :default-percolating-attributes (move cm wh)

  :weights-of-words
  (cond
   ((in (, : . "\"" ";" "'" until after therefore but since)) (int 0))
   ((t) (int 20)))

;  :word-modifier 
;  (let (w)
;    (setq w (reg-sub "[0-9]" "0" g))
;    (get-val w 
;	     (cond ((reg-match "[^a-zA-Z0]+$")
;		    (int 0))
;		   ((in (C_inf C_fin C_vpsc) top15k.txt :hashsize 20011)
;		    (ident))
;		   ((t)
;		    (comp (reg-sub "[A-Z]+" "C" g)
;			  (reg-sub "[a-z]+" "x" g)
;			  (reg-sub "0+" "0" g)
;			  (reg-sub "([^Cx0])[^Cx0]*" "\\1" g)
;			  (reg-sub "^" "U_")
;			  )))))

  :lexical-analyzer English-lexical-analyzer

  :bad-triples badtriples.txt

;  :max-cache-size 500000
)

(def-category Det
  )

(def-category Conn
  )

(def-category Pred
  )

(def-category PreDet 
  )

(def-category PostDet
  )

(def-category NUM
  )

(def-category PRO
  )

(def-category I
  (visible-atts (move cform vform auxform tense pro wh inv comp passive))
  (full-pitem-cond
   (or 
    (pitem-attvec
     (and 
      (or (unifiable (-passive))
	  (contain (+be)))
      (or (not (contain (+perf)))
	  (contain (+have)))))     
    (pitem-preceding-word (in (be is are was were am got get getting see saw have had having)))))  
  )

(def-category V
  (visible-atts (3sg plu vform cform move tense prog govern passive be have auxform lexitem perf))
  (conj-atts (move) (move cform per 3sg))
  (full-pitem-cond
   (pitem-attvec
    (prog
     (if (or (unifiable (-be (vform ing)))
	     (contain (+be +passive +prog (vform ed))))
	 (unify (-govern)))
     (t))))
  )

(def-category V_I
  )

(def-category N
  (visible-atts (move wh whform plu per 3sg genitive nform adv case))
  (conj-atts (move wh adv) (adv move wh nform case))
  (minimal-modifier-cond (not (pitem-attvec (contain ((comp that))))))
  (subcat-atts (genitive adv))
  (full-pitem-cond
   (prog
    (if (not (or (pitem-attvec (contain (+adv)) (contain (+genitive)))
		 (get-modifier-by-type gen)))
	(pitem-attvec (unify (-cm))))
    (if (pitem-attvec (contain (+last_conj +3sg)))
	;; this does not work for 'or'
	(pitem-attvec (modify (-3sg +plu))))
    (t))
   )
  )

(def-category N_C
  (visible-atts (move wh plu per 3sg genitive cat nform adv case ct pron lexitem))
  (subcat-atts (genitive adv))
  )

(def-category NN
  )

(def-category Prep
  (visible-atts (move wh args pform))
  )

(def-category P
  (visible-atts (move wh args pform cat))
  (conj-atts (move wh) (move wh))
  )

(def-category PpSpec
  )

(def-category A
  (visible-atts (move wh adv att prd))
  (conj-atts (barred move wh) (move wh adv nform att))
  )

(def-category THAT
  (visible-atts (wh plu per 3sg nform adv case comp cform ct))
  )

(def-category Have
  (visible-atts (auxform have be perf prog tense cform))
  )

(def-category Aux
  (visible-atts (auxform have be perf prog tense cform))
  )

(def-category Be
  (visible-atts (3sg plu auxform have be perf prog tense cform))
  )

(def-category Subj)

(def-category SentAdjunct
  )

(def-category Interjection)

(def-category Command
  )

(def-category Q
  (visible-atts (move wh bare cform tense comp))
  (conj-atts (move wh comp inv) 
	     (move wh comp cform pro wh inv tense vform))
  (full-pitem-cond
   (and
    (pitem-attvec
     (prog
      (when (unifiable ((cform fin)))
	(unify ((cform fin)))
	(undefine (cm)))
      (when (unifiable ((cform inf)))
	(unify (-guest)))
      (when (and (contain ((cform fin) (vform bare) +3sg))
		 (unifiable (~auxform)))
	(unify (+bare)))
      (and (modify ((cat c)))
	   (not (contain ((move ((slash np))))))
	   (not (contain (-cm))))))
    (pitem-preceding-word (or (not (reg-match [A-Za-z0-9]))
			      (not)
			      (in (but and or so Q question)))))
   )
  )

(def-category C
  (visible-atts (move bare cform tense whform trace comp))
  (conj-atts (move wh inv) 
	     (move wh comp cform pro wh inv tense))
  (full-pitem-cond
   (pitem-attvec
    (prog
     (when (unifiable ((cform fin)))
       (unify ((cform fin)))
       (undefine (cm)))
     (when (unifiable ((cform inf)))
       (unify (-guest)))
     (when (and (contain ((cform fin) (vform bare) +3sg))
		(unifiable (~auxform)))
       (unify (+bare)))
     (and (modify ((cat c)))
	  (not (contain ((move ((slash np))))))
	  (not (contain (-cm))))))
   )
  )

(def-category Adjunct
  )

(def-category COMP
  (visible-atts (auxform have be perf prog tense cform comp))
  )

(def-category Said
  )

(def-category SaidX
  )

(def-category XSaid
  )

(def-category Appo
  (visible-atts (appo))
  )

(def-category VBE
  ;; VBE is the head of clauses where the predicate is not a verb
  (visible-atts (be have auxform vform cform move tense prog lexitem))
  )

(def-category VBE-WH
  ;; VBE is the head of clauses where the predicate is not a verb
  (visible-atts (be have auxform vform cform move tense prog lexitem))
  )

(def-category PwithGap
  (visible-atts (move wh args pform))
  )

(def-category CN
  )

(def-category QBE
  (visible-atts (move wh bare cform tense comp))
  )

(def-category YNQ
  (visible-atts (move wh bare cform tense comp))
  )

(def-modifiers Command
 (? :skip-word (str= ,) 
    (Adjunct
     (dep-type mod-before)
     (modifier-cond
      (pitem-following-word (str= ,))))
    )
 (:head
  (V (dep-type i)
     (modifier-cond
      (pitem-attvec (and (unifiable ((vform bare)))
			 (or (unifiable (~auxform)) (unifiable ((auxform do))))
			 (not (contain ((move () ((slash wh)))))))))
      ))
 )

(def-modifiers V
  (? :skip-word (str= ,)
     (Adjunct (dep-type mod-before)
	      (dep-weight 20.0)
	      (modifiee-cond
	       (pitem-attvec
		(and 
;		 (contain ((cform fin)))
		 (not (and (contain ((vform ing)))
			   (not (contain ((cform fin))))))
		 (not (contain ((auxform to)))))))
	      (modifier-cond 
	       (and (pitem-attvec (unifiable (~move)))
		    (pitem-attvec (not (contain ((pform of)))))
		    (or (pitem-attvec (and (contain ((cat n) +adv))
						(unifiable (-wh))))
			(pitem-attvec (and (contain ((cat a)))
					   (unifiable (+adv))))
			(and (pitem-preceding-word (str= ,))
			     (pitem-following-word (str= ,))))))
	      (merged-cond
	       (prog
;		(if (pitem-following-word (str= ,))
;		    (include-following-word (str= ,)))
		(if (pitem-preceding-word (str= ,))
		    (include-preceding-word (str= ,)))
		(pitem-attvec (unify ((cform fin))))
		))
	      )
     )
  (? :minimal :skip-adjunct
     (Aux (dep-type aux)
	  (percolate auxform have be perf prog tense cform govern)
	  (modifiee-cond
	   (pitem-attvec
	    (and (not (defined (tense)))
		 (or (unifiable (-passive)) (contain (+be)))
		 (or (not (unifiable ((vform ing))))
		     (defined (be)))
		 )))
	  ))
  (? :minimal :skip-adjunct
     (Have (dep-type have)
	   (percolate 3sg plu auxform have be perf prog tense cform)
	   (modifiee-cond
	    (pitem-attvec
	     (and 
	      (unifiable (+perf ~tense))
	      (or (unifiable (-passive)) (contain (+be)))
	      (not (unifiable (-be (vform ing))))
	      )))
	   ))
  (? :minimal :skip-adjunct
     (Be (dep-type be)
	 (percolate 3sg plu auxform have be perf prog tense cform)
	 (modifiee-cond
	  (pitem-attvec
	   (and (not (defined (tense)))
		(or (contain ((vform ing)))
		    (contain (+passive))))))
	 (merged-cond
	  (pitem-attvec
	   (if (contain ((vform ed)))
	       (unify (+passive (move ((slash np)))))
	     (t))	   
	   ))
	 ))
  (? :minimal 
     (Be (dep-type being)
	 (percolate 3sg plu auxform have be perf prog tense cform)
	 (modifier-cond
	   (pitem-head-word (str= being)))
	 (modifiee-cond
	  (pitem-attvec
	   (and (not (defined (tense)))
		(or (contain ((vform ing)))
		    (contain (+passive (move ((cat n)))))))))
	 (merged-cond
	  (pitem-attvec
	   (if (contain ((vform ed)))
	       (unify (+passive (move ((slash np)))))
	     (t))	   
	   ))
	 ))
  (* (A (dep-type amod)
	(modifier-cond
	 (and (pitem-attvec
	       (and (unifiable (+adv -wh))
		    (not (strict-unifiable (barred) ((barred bv))))))
	      (head-last)))
	(modifiee-cond
	 (pitem-attvec (unifiable ((cform fin inf vpsc))))
	 )
	(dep-weight 10.0)
	)
     )
  (:head)
  (* :skip-word (str= ,) 
     (Adjunct (dep-type mod)
	      (dep-weight 20.0)
	      )
     )
  )

(def-conj-constraints V
  (merged-cond
   (cond
    ((and (pitem-modifier (pitem-attvec (unifiable ((vform ing) -be))))
	  (pitem-modifiee (pitem-attvec (and (unifiable (-be))
					     (not (contain ((vform ing))))))))
     (nil)
     )
    ((t)
     (t)))
   )
  )

(def-modifiers N
  (? :minimal 
     (PreDet (dep-type pre)
	     (modifiee-cond (pitem-attvec (unifiable (-pron +det))))
	     ))
  (? :minimal 
     (Det (dep-type det)
	  (minimal-modifier-cond
	   (pitem-head-word (in (the an a their))))
	  (percolate wh whform plu ct +3sg)
	  (modifiee-cond (pitem-attvec (unifiable (-pron +det))))
	  )
     (N (dep-type gen)
	(percolate wh whform)
	(modifiee-cond (pitem-attvec (unifiable (-pron +det))))
	(modifier-cond (pitem-attvec (contain (+genitive))))
	)
     )
  (* :minimal
     (PostDet (dep-type post))
     (NUM (dep-type num)))
  (* :skip-word (str= ,)
     (A (dep-type mod)
	(dep-weight 8.0)
	(block move)
	(modifiee-cond
	 (and (pitem-attvec
	       (not (contain (+pron))))
	      (not (and (head-first)
			(pitem-preceding-word (str= ,))))))
	(modifier-cond
	 (and (pitem-attvec (unifiable (+att -adv)))
	      (or (pitem-attvec (contain (+last_conj)))
		  (head-last)))
	 ))
     (NN (dep-type nn)
	 (dep-weight 10.0)
	 (max-dist 5)
	 (merged-cond
	  (or
	    (not (pitem-head-child
		  (pitem-attvec
		    (contain (+adv)))))
	    (pitem-modifier (pitem-attvec
			     (contain (+adv))))))
	 (modifiee-cond 
	  (and
	   (not (pitem-head-word (reg-match "[0-9.,]+$")))
	   (pitem-attvec
	    (unifiable (-pron -wh)))))
	 (modifier-cond
	  (and (pitem-attvec (unifiable (+att -pron)))
	       (not (pitem-following-word (str= ",")))))
	 ))
  (:head)
  (? :skip-word (str= ,)
     (NUM (dep-type num)
	  (modifiee-cond (pitem-attvec (not (or (contain (+genitive))
						(contain (+pron))))))
	  )
     (A (dep-type pnmod)
	(dep-weight 20.0)
	(modifiee-cond (pitem-attvec (not (contain (+pron)))))
	(modifier-cond 
	 (and (pitem-attvec (unifiable (+att ~move -adv -last_conj)))
	      (not (head-last))))
	))
  (* (P (dep-type mod)
	(modifier-cond (pitem-attvec (or (not (defined (move)))
					 (contain ((node PWithGap))))))
	(modifiee-cond
	 (not (or (pitem-attvec (contain (+pron)))
		  (and (pitem-attvec (contain (+adv)))
		       (not (get-modifier-by-type det)))
		  )))
	(merged-cond
	 (or (not (pitem-modifier (pitem-attvec (contain (+wh)))))
	     (and (pitem-modifiee (head-last))
		  (pitem-modifier (pitem-attvec (contain ((pform of)))))))
	 )
	(dep-weight 20.0)
	(percolate wh whform move)
	))
  (? :skip-word (str= ,)
     (V (dep-type vrel)
	(dep-weight 20.0)
	(check-slash np)
	(merged-cond
	 (prog
	  (if (and (pitem-following-word (str= ,))
		   (pitem-modifier (pitem-preceding-word (str= ,))))
	      (include-following-word (str= ,)))
;	  (pitem-attvec
;	   (if (defined (move))
;	       (undefine (move))))
	  (t)))
	(modifiee-cond 
	 (pitem-attvec (unifiable (-pron -adv -wh -genitive -last_conj)))
	 )
	(modifier-cond
	 (and (or (pitem-following-word (str= ,))
		  (pitem-lexitem (root (in (require))))
		  (not (head-last)))
	      (pitem-attvec
	       (and (defined (move))
		    (unifiable (-be ~auxform -have (vform ed) +passive (move ((nform norm) (slash np)))))
		    (not (contain ((move () ((slash wh))))))
		    (not (contain ((move ((cat c))))))
		    (not (contain ((move ((role expletive))))))))))
	)
     (C (dep-type rel)
	(dep-weight 25.0)
	(modifiee-cond
	 (pitem-attvec (unifiable (-pron -genitive -wh -last_conj))))
	(check-slash wh)
	(merged-cond
	 (prog
	  (if (and (pitem-modifier (pitem-preceding-word (str= ,)))
		   (pitem-following-word (str= ,)))
	      (include-following-word (str= ,)))
	  (and
	   (not (and (pitem-attvec (contain (:inf)))
		     (pitem-modifier (pitem-attvec (contain ((cform inf)))))))
;	   (or
;	    (and (pitem-attvec (contain (+adv)))
;		 (pitem-modifier (pitem-attvec (contain (+adv)))))
;	    (and (not (pitem-attvec (contain (+adv))))
;		 (not (pitem-modifier (pitem-attvec (contain (+adv)))))
;		))
	   )
	  ))
	(modifier-cond 
	 (or (pitem-attvec
	      (and (defined (wh))
		   (or (contain ((whform who)))
		       (contain ((whform whom)))
		       (contain ((whform where)))
		       (contain ((whform whose)))
		       (contain ((whform which))))
		   (unifiable ((cform fin) -inv +wh))
		   )
	       )
	     ;; infinitive clause
	     (and
	      (pitem-attvec (and (unifiable ((cform inf) +pro ~move))
				 (not (contain (+be (vform bare))))))
	      (not (pitem-preceding-word (str= ,)))
	      )

	     ;; -ing form
	     (and (pitem-attvec (unifiable ((cform vpsc) +pro ~move)))
		  (not (pitem-preceding-word (str= ,)))
		  (int-gt (pitem-high) (pitem-low)))

	     ;; finite relative clause with no wh-element
	     (and (pitem-attvec (and (contain ((cform fin) (move (N (slash wh)))))
				     (unifiable (-bare))))
		  (not (pitem-preceding-word (str= ,)))
		  (or (get-modifier-by-type c)
		      (pitem-head-child 
		       (get-modifier-by-type 
			s
			(or (pitem-attvec (contain (+pron)))
			    (pitem-attvec (contain (+pn)))
			    (get-modifier-by-type det)
			    (get-modifier-by-type gen)
			    (get-modifier-by-type pre)
			    (get-modifier-by-type post)
			    ))))
		  )))
	)
     )
  )

(def-modifiers SentAdjunct
  (:head)
  (:skip-word (str= ,)
   (C (dep-type comp1)
      (modifier-cond
       (pitem-attvec (unifiable (~move (cform fin) ~trace -wh)))
       )
      (check-arg 0))
   )
  )

(def-modifiers P
  (? 
     (PpSpec (dep-type p-spec)
	     ))
  (:head
   (Prep (dep-type p)
	 (percolate cat pform args)
	 ))
  ((N (dep-type pcomp-n)
      (modifiee-cond (pitem-attvec (unifiable ((args (N))))))
      (percolate wh whform lexitem)
      (modifier-cond (pitem-attvec (and (not (contain ((cat c))))
					(not (contain ((case nom)))))))
      )
   (C (dep-type pcomp-c)
      (percolate cform)
      (modifier-cond
       (pitem-attvec (and (unifiable (~move))
			  (or (contain ((cform vpsc)))
			      (and (contain (+wh))
				   (not (contain ((whform whom))))
				   (not (contain ((whform whose))))
				   (not (contain ((whform which)))))))))
      )
   )
  )

(def-modifiers A
  (* (A (dep-type mod)
	(modifier-cond (and (pitem-attvec
			     (and (unifiable (+adv (whform how) +att))
				  (not (strict-unifiable (barred) ((barred ba))))))
			    (head-last)))
	(percolate wh whform)
	(dep-weight 8.0)
	)
     )
  (:head)
  (? (P (dep-type mod)
	(dep-weight 20.0)
	(modifiee-cond (pitem-attvec (unifiable (-wh))))
	))
  )

(def-modifiers XSaid
  ((N (dep-type subj)
      (check-slash np)
      (modifier-cond
       (pitem-attvec (unifiable (-adv))))
      ))
  (:head)
  )

(def-modifiers SaidX
  (:head)
  ((N (dep-type subj)
      (check-slash np)
      (modifier-cond
       (pitem-attvec (unifiable (-adv))))
      ))
  )

(def-subclass P
  (PwithGap
   (dep-weight 20.0)
   (percolate move node pform)
   )
  )

(def-subclass N
  (NUM
   (dep-weight 10.0)
   )
  )

(def-subclass Said
  (XSaid
   (dep-weight 20.0)
   )
  (SaidX
   (dep-weight 20.0)
   )
  )

(def-subclass Adjunct
  (N (modifier-cond
      (pitem-attvec (and (defined (adv))
			 (unifiable (+adv -genitive -wh)))
		    ))
     )
  (A (modifier-cond
      (pitem-attvec (unifiable (+adv -wh))
		    ))
     )
  (P (modifier-cond (pitem-attvec (unifiable (-wh)))))
  (Interjection (modifier-cond
		 (and (pitem-preceding-word (str= ,))
		      (pitem-following-word (str= ,))))
		)
  (SentAdjunct )
  (Said )
  (C (modifier-cond
      (and (not (and (head-first) (head-last)))
	   (pitem-attvec
	    (and (unifiable (~move -bare))
		 (or (contain ((comp if whether for)))
		     (and (defined (whform))
			  (not (or (contain ((whform what)))
				   (contain ((whform who)))
				   (contain ((whform whose)))
				   (contain ((whform how)))
				   (contain ((whform which)))))
			  (unifiable (-inv (cform fin))))
		     (contain (+pro)))))))
     )
  (I (modifier-cond
      (pitem-attvec
       (and (unifiable (~move +cm -pro -bare -have (cform vpsc)))
	    (contain ((vform ing))))))
     )
  )
   
(def-modifiers Appo
  (:head
   (N (dep-type head)
      (percolate move wh plu per 3sg genitive nform adv case cm ct lexitem)
      (modifier-cond
       (and
	(pitem-following-word (in (, -- --- _ __ ___ "(" "&mdash;" )))
	(pitem-attvec (and (unifiable (-appo)) (defined (lexitem)))))))
   )
  (? :skip-word (in (, -- --- _ __ ___ "(" "&mdash;" ))
     (Adjunct (dep-type appo-mod)
	(modifier-cond (pitem-attvec (and (contain (+adv))
					  (unifiable (~move -pron)))))
	)
     )
  (:skip-word (in (, -- --- _ __ ___ "(" "&mdash;" ))
   (N (dep-type appo)
      (modifier-cond
       (and
	(and (not (and (pitem-attvec (contain (+last_conj)))
		       (pitem-preceding-word (str= ,))))		  
	     (not (pitem-attvec (contain ((sem (+location))))))
	     (pitem-attvec (unifiable (-wh -pron -appo))))
	(or (pitem-following-word (in (. -- --- _ __ ___ ")" , ";" "\"" ? "&mdash;" )))
	    (not (pitem-following-word)))
	(not (pitem-preceding-word (str= "\"")))))
      (merged-cond
       (prog
	(if (pitem-following-word (in (, -- --- _ __ ___ "(" ")" "&mdash;" )))
	    (include-following-word (in (, -- --- _ __ ___ "(" ")" "&mdash;" ))))
	(pitem-attvec (unify (+appo)))
	(not (and (pitem-modifier (pitem-lexitem (root (reg-match _))))
		  (pitem-head-child (pitem-lexitem (root (reg-match _))))))
	))
      )
   )
  )

(def-subclass N ;; free relative clause
  (C (block wh cform whform cat)
     (dep-weight 20.0)     
     (modifier-cond
      (pitem-attvec (and (unifiable (+wh ~move -adv))
			 (defined (whform))
			 (not (contain ((whform which))))))))
  (I (block cat vform)
     (modifier-cond
      (pitem-attvec
       (strict-unifiable (cform) (~move -pro -bare (cform vpsc)))))
     (dep-weight 20.0) 
     )
  (Appo (dep-weight 20))
  )


(def-modifiers SentAdjunct
  (:head)
  ((C (dep-type comp1)
      (modifier-cond
       (pitem-attvec (unifiable (-inv (cform fin) -bare -wh ~move))))
      ))
  )

  
(def-modifiers CN
  (:head
   (C
    (dep-type cn)
    (head-cond (pitem-attvec (modify (+3sg))))
    (modifier-cond (pitem-attvec
		      (and (unifiable (~move -inv))
			   (or (contain ((comp that)))
			       (contain ((cform inf) (auxform to)))
			       (contain ((cform vpsc) +pro))
			       (and (contain (+wh (whform what)))
				    (defined (trace)))
			       ))))
    )))

(def-subclass Subj
   (CN
    (dep-weight 20)
;    (modifier-cond (pitem-attvec
;		      (and (unifiable (~move -inv))
;			   (or (contain ((comp that)))
;			       (contain ((cform inf) (auxform to)))
;			       (contain ((cform vpsc) +pro))
;			       (contain (+wh (whform what)))
;			       ))))
      )
   (N (modifier-cond
       (pitem-attvec (unifiable syn (~move -wh)))
       )
      )
   )

(def-modifiers C
  (? :minimal
     (THAT
      (dep-type whn)
	(percolate wh whform cm)
	(check-slash np)
	(modifiee-cond
	 (pitem-attvec (and (defined (move))
			    (unifiable ((move ((slash np))))))))
	)
     (N (dep-type whn)
	(percolate wh whform cm)
	(check-slash wh np)
	(modifiee-cond
	 (pitem-attvec (defined (move))))
	(modifier-cond
	 (pitem-attvec (and (defined (wh))
			    (unifiable (+wh ~move)))))
	)
     (A (dep-type wha)
	(percolate wh whform adv)
	(check-slash wh)
	(modifier-cond
	 (pitem-attvec (and (defined (wh))
			    (unifiable (+wh ~move)))))
	(modifiee-cond
	 (pitem-attvec (not (and (defined (move))
				 (unifiable ((move ((slash np)))))))))
	)
     (P (dep-type whp)
	(percolate wh whform)
	(check-slash wh)
	(modifier-cond
	 (pitem-attvec (contain (+wh))))
	(modifiee-cond
	 (pitem-attvec (not (unifiable ((move ((slash np))))))))
	)
     )
  (? :minimal
     (PRO (dep-type s)
	  (modifiee-cond
	   (pitem-attvec (or (contain ((cform inf)))
			     (contain (+be +prog))
			     (contain (+perf +prog))
			     (and (contain ((vform ing)))
				  (unifiable (-be))))))
	  (merged-cond
	   (pitem-attvec
	    (if (contain ((cform inf)))
		(t)
	      (unify ((cform vpsc))))))
	  (percolate pro cform)
	  )
     (COMP (dep-type c)
	   (percolate bare pro auxform have be perf prog tense cform comp wh whform)
	   )
     )
  (? :skip-word (str= ,)
     (Adjunct (dep-type mod)
	      (dep-weight 20.0)
	      (merged-cond
	       (and
		(or (pitem-modifier (pitem-attvec (contain ((cat a)))))
		    (pitem-modifiee (pitem-attvec (unifiable ((move ((slash np))))))))
		(pitem-attvec (unify ((cform fin))))))
	      (modifier-cond
	       (pitem-attvec (and 
			      (not (contain ((pform of))))
			      (unifiable (~move)))))
	      (modifiee-cond
	       (pitem-attvec (unifiable (-pro -bare -wh))))
	      ))
  (:head
   (I (dep-type i)
      (modifier-cond
       (pitem-attvec (and (not (contain ((cform apsc ppsc npsc))))
			  (or (not (contain ((vform ing))))
			      (not (defined (trace)))
			      (contain (+be))
			      (contain (+genitive))))))
      (percolate move cform vform auxform tense pro wh inv comp bare whform have be 3sg cm prog perf)
      ))
  )

(def-modifiers Q
  (? :minimal
     (N (dep-type whn)
	(percolate wh whform cm)
	(check-slash wh np)
	(modifier-cond
	 (pitem-attvec (and (defined (wh))
			    (unifiable (+wh ~move)))))
	(merged-cond
	 (or (pitem-modifiee (pitem-attvec (defined (move))))
	     (pitem-modifier (pitem-attvec (contain (Nadv))))
	     ))
	)
     (A (dep-type wha)
	(percolate wh whform adv)
	(check-slash wh)
	(modifier-cond
	 (pitem-attvec (and (defined (wh))
			    (unifiable (+wh ~move)))))
	)
     (P (dep-type whp)
	(percolate wh whform)
	(check-slash wh)
	(modifier-cond
	 (pitem-attvec (contain (+wh))))
	)
     )
  (:head
   (YNQ (dep-type head)
	)
   (QBE (dep-type head)
	)
   )
  )

(def-modifiers YNQ
  (:minimal
     (Aux  (dep-type inv-aux)
	   (percolate auxform have be perf prog tense cform)
	   (merged-cond
	    (pitem-attvec (modify (+inv -bare (cform fin)))))
	   (modifier-cond (pitem-attvec (not (unifiable ((auxform to))))))
	   (modifiee-cond
	     (pitem-attvec
	      (and (not (defined (tense)))
		   (or (not (unifiable ((vform ing))))
		       (defined (be))
		       ))))
	   )
     (Have (dep-type inv-have)
	   (percolate auxform have be perf prog tense cform vform)
	   (merged-cond
	    (pitem-attvec (modify (+inv -bare (cform fin)))))
	   (modifiee-cond
	     (pitem-attvec
	      (and 
	       (unifiable (+perf ~auxform ~tense) -pro (vform ed))
	       (not (contain (-be (vform ing)))))))
	   )
     (Be   (dep-type inv-be)
	   (percolate auxform have be perf prog tense cform)
	   (modifiee-cond
	    (pitem-attvec
	     (or (contain ((vform ing)))
		 (contain (+passive)))))
	   (merged-cond 
	     (pitem-attvec (modify (+inv -bare (cform fin)))))
	   )
     )
  (:head 
   (I (dep-type head)
      (modifier-cond
       (pitem-attvec (unifiable (~tense ~auxform))))
      )
   )
  )

(def-modifiers QBE
  (:head (Be (dep-type head)
       (modifier-cond (and (pitem-attvec (not (contain (+be +perf))))
			   (or (not (pitem-preceding-word))
			       (pitem-preceded-by A (pitem-attvec (contain (+wh))))
			       (pitem-preceded-by N (pitem-attvec (contain (+wh))))
			       (pitem-preceded-by P (pitem-attvec (contain (+wh))))
			       (pitem-preceding-word (reg-match "[^A-Za-z0-9]")))))
			   
       )
   )
  ((N (dep-type subj)))
  (? :skip-adjunct
   (N (dep-type pred)
      (modifier-cond (get-modifier :cond (t)))
;      (merged-cond (set-trace-att-of-pred))
      )
   (A (dep-type pred)
      (modifier-cond
       (pitem-attvec (unifiable (-adv +prd))))
;      (merged-cond (set-trace-att-of-pred))
      ;; unify the move attribute of the merged item with 
      ;; the attributes of a typical subject of A.
      )
   (P (dep-type pred)
;      (merged-cond (set-trace-att-of-pred))
      )
   (C (dep-type pred)
;      (merged-cond (set-trace-att-of-pred))
      (modifier-cond (pitem-attvec (and (unifiable (~move))
					(or (defined (comp))
					    (contain (+pro))
					    (contain (+wh))))))
      )
   )
  )

(def-subclass Q
  (QBE)
  )

(def-subclass VBE
  (VBE-WH)
  )

(def-modifiers I
  (?
   (Subj (dep-type s)
	 (check-slash np)
	 (percolate cm 3sg plu genitive wh whform)
	 (merged-cond 
	  (prog
	   (if (and (pitem-modifier (pitem-attvec (contain (+3sg))))
		    (pitem-attvec (and (unifiable (-be ~auxform (vform bare)))
				       (not (unifiable ((vform ed)))))))
	       (pitem-attvec (unify (+bare))))
	   (and (pitem-attvec (or (not (contain (+genitive)))
				  (and (unifiable (-be ~auxform (vform ing) (cform vpsc))))))
		(or (pitem-modifier (pitem-attvec (not (defined (cform)))))
		    (pitem-attvec (and (not (contain (+passive)))
				       (not (defined (move))))))
		(or (pitem-modifier (pitem-attvec (not (contain (+genitive)))))
		    (pitem-attvec (contain ((vform ing)))))
		(or (pitem-modifier (pitem-attvec (not (contain ((case acc))))))
		    (pitem-attvec (unifiable ((cform inf)))))
		(or (pitem-modifier (pitem-attvec (contain (N))))
		    (not (pitem-attvec (contain ((cform inf)))))))
	   ))
	 )
   )
  (:head
   (V (dep-type pred)
      ))
  )

(def-subcat VBE V)

(def-subcat V_I V)

(def-subcat N_C N)

(def-modifiers N_C
  (:head)
  (:skip-adjunct
   :skip-word (in ("&quot;" "&ldquo;" "&rdquo;" <ldquo> <rdquo>))
   (C (dep-type comp1)
      (check-arg 0)
;      (percolate move wh plu per 3sg genitive cat nform adv case ct pron lexitem)
      (modifiee-cond (pitem-following-word (in (that the a an any some many most all those it to for "&quot;" "&ldquo;" "&rdquo;" <ldquo> <rdquo>) prons.txt)))
      (modifier-cond (pitem-attvec (unifiable (~move (cform fin inf vpsc)))))
      ))
  )

(def-modifiers V_I
  (:head)
  ((I (dep-type comp1)
      (modifier-cond 
       (pitem-attvec 
	(and
	 (or (contain ((auxform to)))
	     (contain ((vform bare)))
	     (and (contain (+passive)) (not (contain (+be))))
	     (and (contain ((vform ing))) (not (contain (+be))))
	     )
;	 (or (not (contain (+passive)))
;	     (contain (+be (vform ed))))
	 )))
      )
   ))

(def-modifiers VBE
  (:head)
  (:skip-adjunct
   (N (dep-type pred)
      (merged-cond (set-trace-att-of-pred))
      )
   (A (dep-type pred)
      (modifier-cond
       (pitem-attvec (unifiable (-adv +prd))))
      (merged-cond (set-trace-att-of-pred))
      ;; unify the move attribute of the merged item with 
      ;; the attributes of a typical subject of A.
      )
   (P (dep-type pred)
      (merged-cond (set-trace-att-of-pred))
      )
   (C (dep-type pred)
      (merged-cond (set-trace-att-of-pred))
      (modifier-cond (pitem-attvec (and (unifiable (~move))
					(or (defined (comp))
					    (contain (+pro))
					    (contain (+wh))))))
      )
   )
  )

(def-subcat Aeasy A
  (:head)
  (:skip-adjunct
   (C (dep-type comp1)
      (allow-guest)
      (modifier-cond 
       (pitem-attvec
	(contain ((cform inf) (move ((cat n) (slash wh)))))))
      (merged-cond
       (pitem-attvec (undefine (move))))))
  )

(roots
 (C
  (pitem-attvec (and (not (defined (move)))
		     (or (unifiable ((cform fin) -bare))
			 (unifiable ((cform inf)))
			 (unifiable ((cform vpsc))))))
  )
 (Q
  (pitem-attvec (and (not (defined (move)))
		     (or (unifiable ((cform fin) -bare))
			 (unifiable ((cform inf))))))
  )
 (I
  (pitem-attvec (and (contain (+pro (auxform to)))
		     (not (defined (move))))) (int 30)
  )
 (Command
  (pitem-attvec (and (unifiable ((move ((slash np)))))
		     (not (contain ((move () ((slash wh))))))))
  )
 (Adjunct (t))  ;; 
 (N (pitem-attvec (not (defined (move)))) (int 5)
    )
 (P (t) (int 20)
    )
 (A (t) (int 20)
    )
  )
 )

(defun THERE-insertion (item)
  (get-val item 
	   (att (insert-into-avlist 
		 args (attvec syn ((cat n) (nform there))))))
  (get-val item)
  )

(defun IT-insertion (item)
  (when (get-val item (att (contain ((args ((cat c)))))))
    (setq item (duplicate-lexitem (get-val item)))
    (get-val 
     item 
     (att
      (prog
       (rotate-and-insert-into-avlist args (attvec syn ((cat n) (role expletive) (nform it))))
       (cond ((contain (verb (args () ((cat c)))))
	      (unify ((args () (~move)))))
	     ((contain (verb (args () () ((cat c)))))
	      (unify ((args () () (~move)))))
	     )
       )))
    )
  (get-val item)
  )

(defun make-np-move (item)
  (get-val item (att (prog
		      (move-att args move) 
		      (unify ((move ((slash np))))))))
  (get-val item)
  )

(defun make-wh-move (item)
  (if (get-val item (att (contain (verb (args ((cat n)))))))
      (let (duplicate)
	(setq duplicate (duplicate-lexitem (get-val item)))
	(get-val duplicate
		 (prog
		  (att (move-att args move))
		  (att (unify ((move () ((slash wh))))))))
	(assert (get-val duplicate))
	(get-val duplicate)
	))
  )

(defun passivize (item)
  (when (get-val item (att (and (unifiable ((vform ed)))
				(not (defined (passive))))))
    (setq item (duplicate-lexitem (get-val item)))
    (get-val item (att (delete-from-avlist args)))
    (get-val item (att (unify (+passive (vform ed)))))
    )
  (get-val item)
  )

(def-rules English-lexical-rules
  (set-the-category-of-lexitems :act-on-all-matches
   (lexitem (t))
   (prog
    (when (and (litem (att (unifiable (N -wh +att -pron ~case -genitive))))
	       (litem (not (ending s)))
	       (not (exist-lexitem (low) (high)
				   (pred (att (and (contain ((cat a)))
						   (unifiable syn (-adv +att))))))))
      (let (item)
	(setq item (duplicate-lexitem (litem)))
	(if (matching-word (reg-match "[0-9.,]+$"))
	    (get-val item (att (unify ((node NUM)))))
	  (get-val item (att (unify ((node NN))))))
	(get-val item (set-cate (str N)))
	(assert (get-val item))
	)
      )

    (if (not (litem (att (contain (+ref)))))
	(litem (set-cate (att (node-name)))))
    )
   )

  (there-insertion
   (seq (word (str= there))
	(* (lexitem (not (att (contain ((cat n)))))))
	(lexitem (att (and (contain (verb (args ((cat n)))))
			   (not (contain (verb (args ((cat n)) ())))))))
	)
   (assert (THERE-insertion (duplicate-lexitem (litem))))
   )

  (license-PRO
   (seq
    (? (lexitem (and (att (and (contain (A)) 
			       (unifiable (-wh))
			       (contain (+adv))))
		     (not (root (in prep.txt (while when how)))))))
    (lexitem (or (ending -ing)
		 (surform (str= being))))
    )
   (assert
    (make-lexitem (low) (low (-1)) (avm syn (N +pro (check np) (node PRO)))))
   )

  (license-PRO-of-inf
    (lexitem (att (contain ((cat i) (auxform to)))))
    (assert
     (make-lexitem (low) (low (-1)) (avm syn (N +pro (check np) (node PRO)))))
    )

  (verb-variations :act-on-all-matches
   (lexitem (att (contain ((cat v)))))
   (let (passive iti)
     ;; V_X_Y --passive-> V_N 
     (when (litem (att (and (not (defined (passive)))
			    (or (contain ((args () (N))))
				(contain ((args () ((cat c) (cform fin)))))
				(contain ((args () ((cat i)))))))))
       (assert (setq passive (passivize (litem))))
       (assert (IT-insertion (get-val passive)))
       )

     ;; V_C_X --ITI-> V_IT_X_C
     (when (litem (att (contain (verb (args ((cat c)))))))
       (assert (setq iti (IT-insertion (litem))))
       (if (get-val iti (att (and (not (defined (passive)))
				  (contain ((args () (N)))))))
	   (assert (passivize (get-val iti))))
       )
     )
   )

  (make-trace-after-prep
   (seq 
    (lexitem (att (and (contain ((cat p)))
		       (unifiable (~comp)))))
    (or-pat (word (in (can cannot could couldn did do does doesn may
		       might must need ought oughtn shall should will would
		       am was is are be were has have had ? .)))
	    (end-of-sentence)))
   (when (not (and ($ 1 (litem (att (contain ((pform to))))))
		   ($ 2 (matching-word (in (do have be))))))
     (let (item)
       (setq item (duplicate-lexitem ($ 1 (litem))))
       (get-val item (att (modify ((node PwithGap) (move (N (role pcomp-n) (slash np)))))))
       (assert (get-val item))
       ))
   )

  (make-traces :act-on-all-matches
   (lexitem (att (or (contain (verb))
		     (contain ((node VBE) +be)))))
   (let (item)
     (if (litem (att (unifiable (-passive))))
	 (litem (att (unify (-passive)))))
     (cond 
      ((litem (att (or (contain (verb (args ((cat n)))))
		       (contain (verb (args ((cat c))))))))
       (make-wh-move (make-np-move (litem)))
       (if (litem (att (contain (verb -passive (move ((cat n) (nform norm)))))))
	   (make-wh-move (litem))
	 )
       )
      ((litem (att (contain ((node VBE)))))
       (let (duplicate)
	 (setq duplicate (duplicate-lexitem (litem)))
	 (get-val duplicate
		  (prog
		   (att (move-att args move))
		   (att (modify ((node VBE-WH) (move ((slash np)) ((cat n) (role pred) (slash wh))))))))
	 (assert (get-val duplicate))
	 )
       )
      )
     )
   )

;  (small-clause :act-on-all-matches
;   (lexitem (att (and (contain (verb (args ((cat i)))))
;		      (unifiable ((args ((cform apsc vpsc ppsc npsc))))))))
;   (prog
;    (assert (make-lexitem (high (+1)) (high)
;			  (avm syn ((node Ebe) (cform apsc ppsc npsc) +be -govern))))
;    (assert 
;     (make-lexitem 
;      (high (+1)) (high)
;      (avm syn ((node VBE) +be (cform  apsc vpsc ppsc npsc) (move ((slash np) (role subj) (cat n)))))))
;    )
;   )

;  (predicative-adjective :act-on-all-matches
;   (lexitem (att (contain (A (args ((cat c)))))))
;   (prog
;    (if (litem (att (contain ((args ((cat c)))))))
;      (assert (make-np-move (IT-insertion (litem)))))
;    (make-np-move (litem))
;    )
;   )
  )

(def-rules English-lexical-rules 
  (set-proper-name-root :act-on-all-matches
   (lexitem (t))
   (cond
;    ((litem (att (contain ((sem (+corpname))))))
;     (set-lexitem-root (litem) (str _ORG))
;     )
;    ((litem (att (or (contain ((sem (+location))))
;		     (contain ((sem (+city))))
;		     (contain ((sem (+province))))
;		     (contain ((sem (+country)))))))
;     (set-lexitem-root (litem) (str _LOC))
;     )
;    ((litem (att (contain ((sem (+person))))))
;     (set-lexitem-root (litem) (str _PER))
;     )
    ((litem (att (contain ((sem (+time))))))
     (set-lexitem-root (litem) (str _TIME))
     )
    ((litem (att (contain ((sem (+date))))))
     (set-lexitem-root (litem) (str _DATE))
     )
    ((litem (att (contain ((sem (+money))))))
     (set-lexitem-root (litem) (str _MONEY))
     )
    ((litem (att (contain ((sem (+percent))))))
     (set-lexitem-root (litem) (str _PCT))
     )
    ((litem (att (contain ((sem (+price))))))
     (set-lexitem-root (litem) (str _PRICE))
     )
    ((litem (att (contain ((sem (+number))))))
     (if (not (litem (root (str= one))))
	 (set-lexitem-root (litem) (str _NUM)))
     )
    ((litem (att (contain ((sem (+spec))))))
     (set-lexitem-root (litem) (str _SPEC))
     )
    ((litem (att (contain ((sem (+product))))))
     (set-lexitem-root (litem) (str _PROD))
     )
    )
   )
  )

(load cfgrules.lsp)

(defun major-cat ()
  (if (cat (reg-match .*_))
      (cat (reg-sub "_.*$" ""))
    (cat))
  )  

(defun root-word ()
  (if (get-prop type)
      (get-prop type)
    (node2root))
  )  


(def-tree-rules deps-modifier
  (up :name determine-type
      (tree (node2root (reg-match _)))
      (match
       (prog
	(set-prop (type (node2root)))
	(if (node2minp (not (str= "")))
	    (set-root (ident) (node2minp))
	  (set-root (ident) (node2ante (node2minp)))))))


  (up (tree (and (node2root (in (who whom which whose that)))
		 (node2attvec (contain (+wh)))))
      (let (clause wh)
	(setq clause
	      (match 
	       (self-or-exist-ancestor (rel (str= rel)))))
	(when (get-val clause)
	  (setq wh (match))
	  (get-val clause (parent (set-prop (ref (get-val wh)))))
	  (match (set-prop (ante (get-val clause (parent))))))
	))
  (up (tree (cat (str= PRO)))
      (match
       (let (controller this antecedent verb)
	 (when (and (not (node2antecedent))
;		    (not (parent (node2attvec (contain ((vform ing))))))
		    )
	   (setq this (ident))
	   (cond
	    ((parent (rel (str= rel)))
	     (setq controller (parent (parent)))
	     )
	    ((not (parent (rel (str= pred))))
	     (setq verb (get-prop ref (parent (node2root))))
	     (setq controller
		   (parent (parent (exist-c-commander
				    (and (rel (str= s))
					 (cat (reg-match N))
					 (comp
					  (format (get-val verb) "\t" (node2root))
					  (in subj.txt :hashsize 200011))
					 )))))
	     (if (not (get-val controller))
		 (setq controller
		       (parent (parent (exist-c-commander
					(and (rel (str= s))
					     (cat (reg-match N))
					     )))))
	       )))
	   (when (get-val controller)
	     (get-val controller (set-prop (ref (get-val this))))
	     (set-prop (ante (get-val controller))))
	   )
	 (when (rel (str= subj))
	   (setq antecedent (get-prop ante))
	   (if (get-val antecedent)
	       (setq antecedent (get-val antecedent (get-prop ante))))
	   (if (get-val antecedent)
	       (set-root (ident) (get-val antecedent (node2root))))
	   )
	 (set-cat (ident) (str N))
	 )))

  (up (tree (and (cat (or (str= C) (str= CN)))
		 (node2attvec (defined (cform)))))
      (match (set-root (ident) (node2attvec (att-str-val cform)))))

  (up (tree (and (node2root (str= by))
		 (parent (node2attvec (contain (+passive))))))
      (match (set-rel (ident) (str by-subj))))

  (up (node (and (node2attvec (defined (sem)))
		 (not (get-prop type))))
      (comp
       (match)
       (cond
	((node2attvec (contain ((sem (+corpname)))))
	 (set-prop (type (str _ORG)))
	 )
	((node2attvec (contain ((sem (+person)))))
	 (set-prop (type (str _PER)))
	)
	((node2attvec (or (contain ((sem (+location))))
			  (contain ((sem (+city))))
			  (contain ((sem (+province))))
			  (contain ((sem (+country))))))
	 (set-prop (type (str _LOC)))
	 )
	((node2attvec (contain ((sem (+time)))))
	 (set-prop (type (str _TIME)))
	 )
	((node2attvec (contain ((sem (+address)))))
	 (set-prop (type (str _ADDR)))
	 )
	((node2attvec (contain ((sem (+date)))))
	 (set-prop (type (str _DATE)))
	 )
	((node2attvec (contain ((sem (+money)))))
	 (set-prop (type (str _MONEY)))
	 )
	((node2attvec (contain ((sem (+percent)))))
	 (set-prop (type (str _PCT)))
	 )
	((node2attvec (contain ((sem (+price)))))
	 (set-prop (type (str _PRICE)))
	)
	((node2attvec (contain ((sem (+number)))))
	 (set-prop (type (str _NUM)))
	 )
	)
       ))

  ;; unclassified proper name
  (up (node (and (cat (reg-match "[^_]"))
		 (not (node2attvec (defined (sem))))
		 (not (get-prop type))
		 (node2root (reg-match "[A-Z].* [A-Z]"))))
      (match
       (cond ((node2root (exist-word (in orgs.txt)))
	      (set-prop (type (str _ORG)))
	      )
	     ((node2root (exist-word (or (in titles.txt)
					 (reg-match "[A-Z][.]"))))
	      (set-prop (type (str _PER)))
	      ))))

  (up (node (and (node2root (reg-match .*[0-9]))
		 (not (get-prop type))))
      (match
       (cond
	((node2root (reg-match "[$][0-9.][0-9.,]*" "[0-9.][0-9.,]*-yen"))
	 (set-prop (type (str _MONEY))))
	((node2root (reg-match "[0-9.-][0-9,.-]*%"))
	 (set-prop (type (str _PCT)))
	 )
	((node2root (reg-match "[+.]*[0-9][0-9,.]*$"))
	 (set-prop (type (str _NUM))))
	((node2root (reg-match "[$#]?[0-9.]+.*[- ]an?[- ]"))
	 (set-prop (type (str _PRICE))))
	)))

  ;; If a word's root contains white characters other than space,
  ;; replace them with space.
  (up (tree (node2root (reg-match ".*[\n\r\t]")))
      (set-root (match) (match (node2root (reg-sub "[\n\r\t]" " " g))))
      )

  ;; Check whether a hyphenated word with digits ends with a unit or
  ;; moneyunits. If it is, set the type of the node to _SPEC
  ;; (specification) and _MONEY, respectively.
  (up (tree (node2root (reg-match .*[0-9].*-)))
      (match
       (let (unit)
	 (setq unit (node2root (reg-sub ".*[0-9].*-([^-]+)" "\\1")))
	 (cond ((get-val unit (in units.txt))
		(set-prop (type (str _SPEC)))
		)
	       ((get-val unit (in moneyunit.txt))
		(set-prop (type (str _MONEY)))
		)
	       ))))		     
;  ;; for recognized entities of the following type: _TIME, _DATE,
;  ;; _SPEC, _PRICE, _MONEY, _PCT and _NUM, make the type the root form
;  ;; of the word.
;  (up (tree (get-prop type (in (_TIME _DATE _SPEC _PRICE _MONEY _PCT _NUM))))
;      (set-root (match) (match (get-prop type))))

;  ;; If a word's root consists of digits and the characters [./$,%:],
;  ;; replace the digits with @ sign.
;  (up (tree (node2root (reg-match "[0-9./$,%:]+$")))
;      (match
;       (set-root (ident) (node2root (reg-sub "[0-9]" @ g)))
;       ))

  ;; remove argument list from the POS symbol: V_N_N --> V
  (up (tree (cat (reg-match .+_)))
      (set-cat (match) (comp (match (cat)) (reg-sub "_.*$" "")))
      )

  (up (tree (cat (str= VBE))
	    (tree (rel (str= subj)))
	    (tree (rel (str= pred))))
      (let (pred)
	(if (match 2 (cat (str= Prep)))
	    (prog
	     (setq pred (match 2  (or (get-child pcomp-n) 
				      (get-child pcomp-c))))
	     (set-rel (match 1) (format "subj-" (match 2 (node2root)))))
	  (setq pred (match 2)))
	(match 1 (reattach-node (get-val pred)))))


  )

(set-deps-modifier English-parser deps-modifier)

(defmacro dr () (debug-rules English-lexical-rules))


;(debug-weight 1)

