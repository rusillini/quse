(def-att-list sem
  (defattribute address BinAtt)     ;; 
  (defattribute amount BinAtt)     ;; 
  (defattribute city BinAtt)     ;; 
  (defattribute document BinAtt)
  (defattribute construct BinAtt)  ;; man-made structures
  (defattribute corpdesig BinAtt)     ;; 
  (defattribute corpname BinAtt)     ;; 
  (defattribute country BinAtt) ;; name of a country
  (defattribute date BinAtt)
  (defattribute event BinAtt)   ;; NPs that are events
  (defattribute fname BinAtt)   ;; family name
  (defattribute gname BinAtt)   ;; given name
  (defattribute gov BinAtt)
  (defattribute island BinAtt)
  (defattribute lang BinAtt)   ;; language
  (defattribute location BinAtt)
  (defattribute male BinAtt)
  (defattribute money BinAtt)
  (defattribute number BinAtt)
  (defattribute other BinAtt)
  (defattribute percent BinAtt)
  (defattribute person BinAtt)
  (defattribute phone BinAtt)
  (defattribute post BinAtt)
  (defattribute price BinAtt)
  (defattribute product BinAtt)
  (defattribute province BinAtt);; name of a province
  (defattribute sea BinAtt)
  (defattribute spec BinAtt)
  (defattribute time BinAtt)
  (defattribute title BinAtt)   ;; title words such as Mr., Prof.
  (defattribute unit BinAtt)   ;; 
  )

(def-att-list syn
  (defattribute 3sg BinAtt)     ;; third person singular
  ;; verbs with -s ending have the +sg attribute
  ;; pronouns 'he', 'she', and 'it' also have the +3sg attribute
  ;; Plural nouns has the -3sg attribute.

  (defattribute allcap BinAtt)  ;; all letters are capitalized
  ;; a word usage has +allcap attribute if all of is letters must be 
  ;; capitalized. For example, IN as an appreviation of Indiana, have the
  ;; +allcap attribute so that when the input text contains 'in' or 'In', 
  ;; the parser doing not have to consider the possibility that they are nouns.

  (defattribute adv BinAtt)     ;; adverbial
  ;; Both adjective and adverbs belong to category A (with (cat a) attribute
  ;; They are distinguished by this attributes. Adjectives have the -adv
  ;; attribute. Adverbs have the +adv attribute. If a word can be both 
  ;; adjective and adverb, its adv attribute is left underfined. An example
  ;; of such a word is 'early'

  (defattribute appo BinAtt)    ;; +appo 
  ;; this attribute is created during parsing. An appositive have the 
  ;; +appo attribute. This attribute is used to make sure that an 
  ;; appositve does not have an appositive modifier itself.
  ;; For example, given
  ;;    Felix Shen, president of Acme, John Smith, and Jane Doe ...
  ;; If the parser analyzes "president of Acme" as an appositive modifer
  ;; of "Felix Shen", it won't analyze "John Smith" as an appositive of
  ;; "president of Acme". 

  (defattribute att BinAtt)     ;; attributive
  ;; There are two kinds of adjectives: attributive and predicative.
  ;; Attributive adjectives can be used as a modifier of a noun. Predicative
  ;; adjectives can be used as a predicate. Most adjectives can be used in both
  ;; ways. For example, "a fancy car" (attributive), and "the car is
  ;; really fancy" (predicative). Some adjectives can only be used in one way.
  ;; For example, "accompanying" can only be used attributively and "afraid"
  ;; can only be used predicatively. So "accompanying" has (+att -prd) and
  ;; "afraid" has (-att +prd).

  (defattribute be BinAtt)      ;; a form of be
  ;; Variations of "be", such as "be", "is", "were", have the +be attribute.

  (defattribute bare BinAtt)    ;; bare clause
  ;; some verbs require there finite clausal complements to be 'bare'.
  ;; For example: "I demand that he resign". The clause "that he resign"
  ;; has a +bare attribute. Most verbs that take finite clause as a 
  ;; complement (e.g., 'think', 'know', ...) has -bare attribute inside the
  ;; args feature. So they won't take clauses such as "that he resign" as
  ;; complement.

  (defattribute cap BinAtt)     ;; should be spelt capitalized
  ;; If a word usage has this feature, this usage is only possible when
  ;; the word is capitalized in the input text. For example, 
  ;; the lexical entry for the word 'how' is:
  ;; (how
  ;;  (syn (A +adv +wh (whform how)))
  ;;  (syn (PN (sem (+gname)) +cap (rare very)))
  ;;  (freq A 5 N 17)
  ;;  (phrases (how many) (how much) (how, God knows) (how, no matter))
  ;;  )
  ;; the second usage has the +cap attribute, which means that 'how' can only
  ;; be a given name (that's what +gname means) when 'how' is capitalized.

  (defattribute cm BinAtt)      ;; -cm means the source needs case marking
  ;; This attribute is used to implement Case Filter, which states that
  ;; all overt noun phrases must be case marked.

  (defattribute cmp BinAtt)     ;; has comparative form
  ;; If an adjective has +cmp, it has a comparative form.
  ;; For example, 'hot' has +cmp, but 'national' doesn't

  (defattribute cn BinAtt)      ;; is compound noun
  ;; If a lexical item has +cn, it is a compound noun. For example,
  ;; 'army hut' has +cn.

  (defattribute control BinAtt) ;; a parser internal attribute
  ;; don't mess with this.

  (defattribute ct BinAtt)      ;; countable (noun)
  ;; Countable nouns have +ct. Uncountable nouns have -ct. If a noun may be
  ;; used as a either countable or uncountable noun, the ct attribute is left
  ;; unspecified

  (defattribute det BinAtt)     ;; determiner
  ;; determiners have +det. Nouns that do not allow any determiners, e.g.,
  ;; pronouns, have -det.

;  (defattribute easy BinAtt)    ;; like the word 'easy'
  ;; The 'easy' and some other words like 'difficult' and 'tough', are rather
  ;; special in terms their treatment in linguistics. This attribute is used
  ;; to identify this group of adjectives.

;  (defattribute free_rel BinAtt);; free relative clause

  (defattribute genitive BinAtt);; genitive pronoun
  ;; genitive nouns have +genitive. Examples: their, Kim's

  (defattribute govern BinAtt)  ;; head is a governor
  ;; This is an attribute used by the parser

  (defattribute group BinAtt)   ;; nouns that can be both singular and plural
  ;; Examples: committee, crew, fish

  (defattribute guest BinAtt)   ;; to indicate a phrase is an adjunct
  ;; A guest is an element (typically adverbial modifier) that is freely
  ;; positioned. For example, "the company announced yesterday that the
  ;; product line has been canceled". The word 'yesterday' is a 'guest'
  ;; modifier of 'announced'.

  (defattribute have BinAtt)    ;; a form of have
  ;; +have indicates that word is a form of 'have' and is used 
  ;; as a auxilary verb. 

  (defattribute head_final BinAtt) ;; 
  ;; head_final is used to indicate the position of the head relative to
  ;; the complements. For the English language, the default is -head_final.
  ;; For SOV langauges, the default would be +head_final

  (defattribute inv BinAtt)     ;; inverted auxilary verb
  ;; to indicate the auxilary and and the subject have been inverted, as
  ;; in questions. Example: "Have you been dring?"

  (defattribute last_conj BinAtt)  
  ;; and/or are +last_conj, either/both are -last_conj
  
  (defattribute neg BinAtt)     ;; negation
  ;; the negated forms of auxilaries, 'be', and 'have' have this
  ;; attribute: couldn't, isn't

  (defattribute nilTo BinAtt)
  ;; verbs that are followed by an covert 'to' have +nilTo
  ;; E.g., help, wanna, gotta

  (defattribute opt BinAtt)     ;; optional argument
  ;; this is usually used inside the 'args' feature. If an element in the
  ;; feature has this attribute, the corresponding argument is optional.
  ;; For example, the object of 'cook' is optional.

  (defattribute passive BinAtt) ;; passive voice
  ;; +passive indicates that a verb or clause is in passive voice.
  ;; if a verb has -passive in the dictionary, it means that the verb should
  ;; not be used in a passive sentence. For example, "I want him to leave"
  ;; cannot be passivized as *"He was wanted to leave". This particular usage
  ;; of 'want' has the -passive feature. When 'want' is used as a transitive
  ;; verb, it can be in passive voice, as in "help was wanted".

  (defattribute perf BinAtt)    ;; perfective aspect
  ;; +perf indicates a verb or a clause has the perfective aspect.
  ;; -perf means that the verb or a clause cannot be perfective.

  (defattribute phrase BinAtt)  ;; delete smaller lexical entries
  ;; If a phrasal word has +phrase, a lexical rule will remove all possible
  ;; usages of the words that formed part of the phrasal word, so that the
  ;; only reading of this sequence of word is the phrasal word.

  (defattribute plu BinAtt)     ;; plural

  (defattribute pn BinAtt)      ;; proper noun
  ;; +pn means that a noun is a proper name.

  (defattribute postnom BinAtt) ;; Post nominal adjective

  (defattribute prd BinAtt)     ;; predicative
  ;; see documentation of 'att'

  (defattribute pro BinAtt)     ;; PRO subject
  ;; The empty category PRO has the +pro attribute. If the clause has
  ;; +pro, it has a PRO as its subject.

  (defattribute prog BinAtt)    ;; progressive aspect
  ;; A clause has +prog if it has progressive aspect

  (defattribute pron BinAtt)    ;; pronoun
  ;; +pron attribute is assigned to nouns with +pron attribute

  (defattribute ref BinAtt)     ;; reference entry
  ;; used to deal irregular verbs

  (defattribute refl BinAtt)    ;; reflective pronoun
  ;; Examples: myself, themselves, ... 

  (defattribute wh BinAtt)      ;; wh-element
  ;; wh words have +wh attribute. If a clause have +wh, contains a wh-element.
  ;; if the 'args' attribute of a verb contains -wh, the verb do not allow
  ;; wh-clauses as the argument. For example, *"I think who came".

  (defattribute auxform EnumAtt    ;; the forms auxilary verb
    (allowable-values 
     to can could dare do did does may might would should must will ought 
     shall have_to be_going_to need had_better
     ))

  (defattribute cat EnumAtt        ;; major category
    (allowable-values 
     c ;; full clause
     i ;; small clause, clauses without complementizer
     n ;; noun
     v ;; verb
     a ;; adjective or adverb (depending on +adv or -adv)
     p ;; preposition
     d ;; determinier
     j ;; conjunction
     ))

  (defattribute pform EnumAtt      ;; the preposition form
    (allowable-values aboard about above according_to across afore
      after against agin along alongside amid amidst among amongst anent
      around as aslant astride as_opposed_to at athwart bar because_of
      before behind below beneath beside besides between betwixt beyond but
      by circa despite down during due_to ere except for from in inside into
      less like mid midst minus near next nigh nigher nighest
      notwithstanding of off on on_to onto out out_of outside over past
      pending per plus qua re round sans save since thanks_to through
      throughout thru till to toward towards under underneath unlike until
      unto up upon versus via vice while with within without other)
    )

  (defattribute vform Disj8Att      ;; inflection of verb
    (allowable-values bare s ed ing))

  (defattribute role EnumAtt      ;; 
    (allowable-values 
     subj ;; subject
     scsubj ;; subject of small clause
     obj  ;; object
     obj1 ;; first object
     obj2 ;; second object
     sc   ;; small clause
     fc   ;; full clause
     dest ;; destination
     desc ;; description
     pcomp-n ;; complement of preposition
     mod  ;; modifier
     pred ;; predicate of BE
     expletive))
  ;; the role attribute specifies the role of an argument. For
  ;; example, The verb 'solve' takes two argument, the role of first
  ;; argument is subj and the role of the second argument is 'obj'.
  ;; The word 'solve' is 'Tn' which is defined as the following bundle
  ;; of attrubutes:
  ;; (alias Tn    
  ;;  (make-av (verb (args (N -adv (role subj)) (N -adv (role obj)))))
  ;;  )

  (defattribute rare EnumAtt       ;; the rarity of lexical items
    (allowable-values very very_very))
  ;; If a word usage have (rare very) or (rare very_very), the parser
  ;; assigns extra weight to the corresponding lexical item. Parse
  ;; trees involving such items are less preferred.  For example, the
  ;; word 'can' can be an auxiliary, a verb and a noun.  The auxiliary
  ;; usage is by far the most frequent. The noun and verb usage of
  ;; 'can' have the attribute (rare very) so that they are less
  ;; preferred by the parser.
    
;;  (defattribute pred Disj8Att       ;; type of predicate
;;    (allowable-values n v a p c))
;;  ;;

  (defattribute barred Disj8Att      ;; barred as these types modifiers
    (allowable-values ba aa bv av))
  ;; This attribute is mostly used by adverbs
  ;; If an adverb has (barred ba), it cannot be used before an adjective
  ;; If an adverb has (barred aa), it cannot be used after an adjective
  ;; If an adverb has (barred bv), it cannot be used before a verb
  ;; If an adverb has (barred av), it cannot be used after a verb

  (defattribute slash Disj8Att       ;; types of movement
    (allowable-values np wh))
  ;; An attribute used by the parser to implement feature checking in
  ;; Minimalist Program.
  ;; check and slash mush have the same set of values

  (defattribute check Disj8Att
    (allowable-values np wh))
  ;; An attribute used by the parser to implement feature checking in
  ;; Minimalist Program.
  
  (defattribute nform Disj8Att      ;; the type of NP
    (allowable-values there it norm))
  ;; Only one noun may have the (nform there) attribute: 'there'
  ;; Only one noun may have the (nform it) attribute: 'it'
  ;; All the other nouns have the attribute (nform norm).

  (defattribute case Disj8Att       ;; the case of an NP
    (allowable-values acc nom dat gen))
  ;; (case acc): accusitve case, assigned to nouns that are the objects
  ;;             of verbs and prepositions
  ;; (case nom): nominative case, assigned to nouns at subject positions
  ;; (case dat): dative case
  ;; (case gen): genitive case

  (defattribute cform Disj8Att      ;; the type of clauses
    (allowable-values fin inf npsc apsc ppsc vpsc))
  ;; (cform fin): finite clause, e.g., I think [the key is lost]
  ;; (cform inf): infinitive clause, 
  ;;      e.g., I wanted [to sleep]. I believe [him to be a good candidate]
  ;; (cform npsc): small clause where the predicate is a noun phrase
  ;;      e.g., I consider [him a good candidate]
  ;; (cform ppsc): small clause where the predicate is a prepositional phrase
  ;;      e.g., I consider [it in good condition]
  ;; (cform apsc): small clause where the predicate is a adjectival phrase
  ;;      e.g., I consider [it good]
  ;; (cform vpsc): small clause where the predicate is a -ing verb phrase
  ;;      e.g., I saw [them leaving the garden]

  (defattribute comp Disj8Att       ;; the type of complimentizers
    (allowable-values for that whether if other none))
  ;; The above words are the only ones that have the comp attribute.

  (defattribute per Disj8Att        ;; person
    (allowable-values 1 2 3))
  ;; (per 1): I, me, my, we, us, our ...
  ;; (per 2): you, your, yours,
  ;; (per 3): he, she, they, them, ...

  (defattribute tense Disj8Att      ;; tense
    (allowable-values present past future pastfut)) 
  ;; The tense attribute specifies the tense of a clause. The tense of
  ;; infinitive and small clauses are undefined.

  (defattribute whform DisjAtt     ;; form of wh-element
    (allowable-values 
        what where who whose which when why whom whoso whosever
        how however whatever whichever whoever whichsoever whosoever whatsoever
	whomever whomsoever wherever whenever))
  ;; The above words are the only ones that have the whform attribute.

  (defattribute lexitem EntAtt)    ;; the lexical item 
  ;; A pointer to the LexItem object.  This attribute is used by
  ;; parser and should NEVER be specified in the lexicon

  (defattribute trace TraceAtt) ;;
  ;; to indicate the position of the trace

  (defattribute node StrAtt)       ;; Corresponding node in the grammar network

  (defattribute sem VecAtt (interpreter sem))
  ;; the semantic properties of the word

  (defattribute args StackAtt)
  ;; the list of arguments of a word (including the subject).

  (defattribute move StackAtt)
  ;; the list of descriptions of the moved elements.
  )

  
