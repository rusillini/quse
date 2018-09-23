(alias verb (make-av ((cat v) -cap)))
;; all verbs

(alias N (make-av ((cat n) (nform norm))))
;; all nouns except expletive 'it' and 'there'

(alias obj_np (make-av ((cat n) (nform norm) (case acc))))
;; noun with accusative case

(alias pp (make-av ((cat p) -passive -cap)))
;; prepositions

(alias cp (make-av ((cat c) -cap)))
;; C clauses

(alias present-be ;; is, am, are
       (make-av ((cat i) ~auxform (tense present) -perf +be (cform fin) -cap)))
;; forms of 'be' in present tense

(alias past-tense (make-av ((cform fin) (vform ed) -prog -perf -passive (tense past))))
;; verbs in past tense

(alias past-participle (make-av ((vform ed) -prog -passive +perf)))
;; past participles of verbs, which is used in perfective aspect.
;; e.g., 'have discovered ...', 

(alias passive-voice (make-av ((vform ed) -prog +passive)))
;; passive form of verbs

(alias PreDet ;; predeterminer
  (make-av ((cat d) (node PreDet)))
  )
;; predeterminers are determiners that could appear before an ordinary
;; determiner. For example, '*such* a good boy', '*all* the participants.

(alias PostDet ;; postdeterminer
  (make-av ((cat d) (node PostDet)))
  )
;; postdeterminers are determiners that could appear after an ordinary
;; determiner. For example, in 'a few examples', 'few' is a postdeterminer

(alias Det (make-av ((cat d))))
;; determiners

(alias seem
  (make-av ((cat v) (args ((cat i) (cform inf apsc) -passive -bare (role sc)))))
  )

(alias seemthat
  (make-av (verb -passive (args ((cat n) (nform it) (role expletive)) ((cat c) (cform fin) (comp that) (role fc)))))
  )

(alias :fin
 (make-av ((args ((cat c) -inv -wh ~move (cform fin)))))
 )

(alias :bare_cp
 (make-av ((args (+bare))))
 )

(alias ::bare_cp
 (make-av ((args () (+bare))))
 )

(alias :w
 (make-av ((args ((cat c) +wh (comp for none if whether) (cform fin)))))
 )

(alias :inf
 (make-av ((args ((cat c) (cform inf)))))
 )

(alias :ft
 (make-av ((args ((cat c) (cform fin inf)))))
 )

(alias :opt
 (make-av ((args (+opt))))
 )

(alias ::opt
 (make-av ((args () (+opt))))
 )

(alias :opt:
 (make-av ((args (+opt) ())))
 )

(alias :to
 (make-av ((args ((cat p) (pform to)))))
 )

(alias :for
 (make-av ((args ((cat p) (pform for)))))
 )

(alias :on
 (make-av ((args ((cat p) (pform on)))))
 )

(alias :about
 (make-av ((args ((cat p) (pform about)))))
 )

(alias :at
 (make-av ((args ((cat p) (pform at)))))
 )

(alias :of
 (make-av ((args ((cat p) (pform of)))))
 )

(alias :in
 (make-av ((args ((cat p) (pform in)))))
 )

(alias :from
 (make-av ((args ((cat p) (pform from)))))
 )

(alias :out
 (make-av ((args ((cat p) (pform out)))))
 )

(alias :against
 (make-av ((args ((cat p) (pform against)))))
 )

(alias :after
 (make-av ((args ((cat p) (pform after)))))
 )

(alias :with
 (make-av ((args ((cat p) (pform with)))))
 )

;; change La to Ia
;;(alias La     ;;
;;  (make-av ((cat v) -wh -cap -passive (args (N) ((cat a) -adv))))
;;  )
;;

(alias I      ;;
  (make-av (verb (args (N (role subj))) -passive))
  )

(alias I[a]   ;;
  (make-av (I (args () ((cat a) -adv +opt (role desc)))))
  )

(alias Tn     ;;
  (make-av (verb (args (N (role subj)) (N (role obj)))))
  )

(alias Tn-pas     ;;
  (make-av (Tn -passive))
  )

(alias Tn.[pr]  ;;
  (make-av (verb (args (N -adv (role subj)) (N (role obj)) (pp  +opt (role mod)))))
  )

(alias T[n.pr]  ;;
  (make-av (verb (args (N -adv (role subj)) (N +opt (role obj)) (pp  +opt (role mod)))))
  )

(alias Tc   ;;
  (make-av (verb (args (N -adv (role subj)) (cp (cform fin inf vpsc) (role fc)))))
  )

(alias Tfw     ;;
  (make-av (Tc (args () ((cform fin)))))
  )

(alias Tf     ;;
  (make-av (Tfw (args () (-wh))))
  )

(alias Tw     ;;
  (make-av (Tfw (args () (+wh (comp for none if whether)))))
  )

(alias Tnpfin   ;;
  (make-av (verb (args (N -adv (role subj)) (N (role obj)) (cp (cform fin) (role fc)))))
  )

(alias Tinf     ;;
  (make-av (Tc (args () ((cform inf)))))
  )

(alias Tt     ;;
  (make-av (Tinf (args () (+pro -wh))))
  )

(alias Ttw    ;;
  (make-av (Tinf (args () (+pro))))
  )

(alias Tftg   ;;
  (make-av (Tc (args () ((cform fin inf vpsc)))))
  )

(alias Twt    ;;
  (make-av (Tftg (args () ((cform fin inf) +wh (comp none)))))
  )

(alias Tft    ;;
  (make-av (Tftg (args () ((cform fin inf) -wh))))
  )

(alias Tfg    ;;
  (make-av (Tftg (args () ((cform fin vpsc) -wh))))
  )

(alias Tfwt   ;;
  (make-av (Tftg (args () ((cform fin inf)))))
  )

(alias Tfwg   ;;
  (make-av (Tftg (args () ((cform fin vpsc)))))
  )

(alias Ttg    ;;
  (make-av (Tftg (args () ((cform inf vpsc) -bare +pro -wh))))
  )

(alias Tg     ;;
  (make-av (Tftg (args () ((cform vpsc) +pro -wh))))
  )

(alias Tip    ;;
  (make-av (verb (args (N -adv (role subj)) ((cat i) -pro (role sc)))))
  )

(alias Tsg    ;;
  (make-av (Tip (args () ((cform vpsc)))))
  )

(alias Cni    ;;
  (make-av (Tip (args () ((cform inf) ~auxform (vform bare)))))
  )

(alias Cnig   ;;
  (make-av (Tip (args () ((cform inf vpsc) ~auxform))))
  )

(alias Cnian   ;;
  (make-av (Tip (args () ((cform inf apsc npsc) ~auxform))))
  )

(alias Cn.a   ;;
  (make-av (verb (rare very) (args (N -adv (role subj)) (N -adv (role obj1)) ((cat a) -adv ~move (role desc)))))
  )

(alias Cn.n   ;;
  (make-av (verb (rare very) (args (N -adv (role subj)) (N -adv (role obj1)) (N -adv (role desc)))))
  )

(alias Cn.t   ;;
  (make-av (Tip (args () ((cform inf) -prog -bare))))
  )

(alias Cn.g   ;;
  (make-av (Tip (args () ((cform vpsc)))))
  )

(alias Cn.tg   ;;
  (make-av (Tip (args () ((cform inf vpsc) -bare))))
  )

(alias Dn.n   ;;
  (make-av (verb (rare very) (args (N -adv (role subj)) (N -adv (role obj1)) (N -adv (role obj2)))))
  )

(alias Dn.pr  ;;
  (make-av (verb (args (N -adv (role subj)) (N -adv (role obj)) (pp (role dest)))))
  )

(alias Dn.fwt ;;
  (make-av (verb (args (N -adv (role subj)) (N -adv (role obj)) (cp +pro (cform fin inf) (role fc)))))
  )

(alias Dn.f   ;;
  (make-av (Dn.fwt (args () () ((cform fin) -wh))))
  )

(alias Dn.ft  ;;
  (make-av (Dn.fwt (args () () (+pro -wh))))
  )

(alias Dn.w   ;;
  (make-av (Dn.fwt (args () () ((cform fin) +wh (comp for none if whether)))))
  )

(alias Dn.fw  ;;
  (make-av (Dn.fwt (args () () ((cform fin)))))
  )

(alias D[n].fwt ;;
  (make-av (Dn.fwt (args () (+opt) ())))
  )

(alias D[n].fw   ;;
  (make-av (D[n].fwt (args () () ((cform fin)))))
  )

(alias D[n].f   ;;
  (make-av (Dn.f (args () (+opt) ())))
  )

(alias D[n].ft   ;;
  (make-av (Dn.ft (args () (+opt) ())))
  )

(alias Dpr.cp  ;;
  (make-av (verb (args (N -adv (role subj)) (pp (pform to) (role dest)) (cp (cform fin inf) (role fc)))))
  )

(alias Dpr.f  ;;
  (make-av (Dpr.cp (args () () ((cform fin) -wh))))
  )

(alias Dpr.w  ;;
  (make-av (Dpr.cp (args () () ((cform fin) +wh))))
  )

(alias Dn.t   ;;
  (make-av (Dn.fwt (args () () ((cform inf) -bare +pro -wh))))
  )

(alias Dn.tw  ;;
  (make-av (Dn.fwt (args () () ((cform inf) -bare +pro +wh))))
  )

(alias Dpr.t  ;;
  (make-av (Dpr.cp -passive (args () () ((cform inf) -bare -wh))))
  )

(alias D[pr.t]  ;;
  (make-av (Dpr.t (args () (+opt) (+opt))))
  )

(alias Dn.pr.as  ;;
  (make-av (Dn.pr (args () () ((pform as)))))
  )

(alias Dn.pr.with  ;;
  (make-av (Dn.pr (args () () ((pform with)))))
  )

(alias Dn.pr.of  ;;
  (make-av (Dn.pr (args () () ((pform of)))))
  )

(alias ItTnf
  (make-av (verb (args ((cat c) (cform fin inf) (role fc)) (N -adv (role obj)))))
  )

(alias T[n]   ;;
  (make-av (Tn (args () (+opt))))
  )

(alias Dn.[n]   ;;
  (make-av (Dn.n (args () () (+opt))))
  )

(alias Tia  ;;
  (make-av (Tip (args () ((cform apsc inf) -bare))))
  )

(alias Tian  ;;
  (make-av (Tip (args () ((cform npsc apsc inf) -bare))))
  )

(alias Tiap  ;;
  (make-av (Tip (args () ((cform ppsc apsc inf) -bare))))
  )

(alias Tan   ;;
  (make-av (Tip (args () ((cform npsc apsc) -bare))))
  )

(alias Tiang ;;
  (make-av (Tip (args () ((cat i) (cform inf apsc npsc vpsc) -bare))))
  )

(alias Tianp ;;
  (make-av (Tip (args () ((cat i) (cform inf apsc npsc ppsc) -bare))))
  )

(alias Tag ;;
  (make-av (Tip (args () ((cat i) (cform apsc vpsc)))))
  )

(alias Tng ;;
  (make-av (Tip (args () ((cat i) (cform npsc vpsc)))))
  )

(alias Tiag ;;
  (make-av (Tip (args () ((cat i) (cform inf apsc vpsc) -bare))))
  )

(alias Tig ;;
  (make-av (Tip (args () ((cat i) (cform inf vpsc) -bare))))
  )

(alias C ;;
  (make-av (N +ct))
  )

(alias Cs ;;
  (make-av (C -plu +3sg))
  )

(alias Cp ;;
  (make-av (C +plu))
  )

(alias U ;;
  (make-av (N -ct -plu))
  )

(alias PN ;;
  (make-av (N +pn +cap)))

(alias Npl ;; Name of places
  (make-av ((cat n) (nform norm) +pn +cap (sem (+location)))))

(alias Nadv ;;
  (make-av (N +adv) ))

(alias A  ;;
  (make-av ((cat a)))
  )

(alias Adv ;;
  (make-av ((cat a) +adv -prd)))

;(alias AdvS ;;
;  (make-av (Adv +adv_s -adv_v))
;  )
;
;(alias AdvV ;;
;  (make-av (Adv -adv_s +adv_v))
;  )
;
(alias Adj ;;
  (make-av (A -adv)))

(alias Ap ;;
  (make-av (A +att +prd)))

(alias Aa ;;
  (make-av (A +att -prd)))

(alias Aatt ;;
  (make-av (Adj +att -prd))
  )

(alias Aprd ;;
  (make-av (Adj -att +prd))
  )

(alias Aftw ;;E2a
  (make-av (Adj (args ((cat c) (role fc) +opt (cform fin inf)))))
  )

(alias Aft ;;E2a
  (make-av (Aftw (args (-wh))))
  )

(alias Af ;;E2a
  (make-av (Aft (args ((cform fin)))))
  )

(alias At ;;E2a
  (make-av (Aft (args ((cform inf) +pro -bare))))
  )

(alias ItAftw ;;
  (make-av ((cat a) -adv +prd (args ((cat c) ~move (role subj) (cform fin inf)))))
  )

(alias ItAft ;;
  (make-av (ItAftw (args (-wh))))
  )

(alias ItAf ;;E2b
  (make-av (ItAft (args ((cform fin)))))
  )

(alias ItAsbj ;;
  (make-av (ItAf (args ((auxform should)))))
  )

(alias E2biii ;;
  (make-av (ItAftw (args () (pp (pform to) +opt))))
  )

(alias Aeasy ;;
  (make-av (Adj :inf (args ((move (N (slash wh)))))))
  )

(alias Aniceof ;;
  (make-av (Aprd -cmp (args ((cat c) (cform inf)) ((cat p) (pform of)))))
  )

(alias Prep ;;
  (make-av ((cat p) (args ((case acc) (nform norm))) (node Prep)))
  )

(alias Pron ;;
  (make-av (N +pron -det)))

(alias Conj ;;
  (make-av ((cat j))))

(alias Conn ;;
  (make-av (~cat (args ((cat c) -bare -wh (comp none))) (node SentAdjunct))))

(alias Sentsub ;;
  (make-av (~cat +adv))) 

(alias ConnThat ;;
  (make-av (~cat (args ((cat c) -bare (comp that))) (node SentAdjunct)))
  )

(alias ConnTo ;;
  (make-av (~cat (args ((cat c) (auxform to))) (node SentAdjunct)))
  )

