(def-category AsAs)

(def-category AsP)

(def-category As)

(def-modifiers AsAs
  ((As (dep-type as1)))
  (:head
   (A (dep-type as-head)
      (percolate cat lexitem)
      ))
  ((As (dep-type as2)))
  ((N (dep-type as-arg))
   (A (dep-type as-arg))
   )
  )

(def-modifiers AsP
  (:head (As (dep-type as1)
	     (percolate cat lexitem)))	 
  ((A (dep-type comp1)
      (modifier-cond
       (pitem-attvec (unifiable (-adv)))))
   )
  )

(def-category ItemList)

(def-category ItemListTail)

(def-category Semicolon)

(def-modifiers ItemList
  (:head (N))
  ((Semicolon (dep-type semicolon)))
  (? (ItemList (dep-type conj)))
  )

(def-subclass A
  (AsAs)
  )

(def-subclass P
  (AsP)
  )

(def-subclass N
  (ItemList)
  )

