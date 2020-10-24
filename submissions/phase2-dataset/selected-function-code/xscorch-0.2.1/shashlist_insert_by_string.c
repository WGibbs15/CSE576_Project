shashlist_item *shashlist_insert_by_string(shashlist *list, void *data, int class, const char *key) {
/* shashlist_insert_by_string
   Append a new data item onto a hash dll, keyed by string.
   This will return NULL if the item is a duplicate or if allocation fails. */

   int hashindex;
   shashlist_item *item, *temp;

   if(list == NULL || data == NULL || key == NULL)
      return(NULL);

   item = shashlist_item_new(data, 0, key, class);
   if(item == NULL)
      return(NULL);

   /* Add the def to the dll */
   if(list->head == NULL) {
      /* Empty list */
      list->tail = list->head = item;
   } else {
      /* Find the proper place in the list. */
      temp = list->head;
      while(temp->next != NULL && strcomp(temp->next->keystr, key) <= 0)
         temp = temp->next;
      /* Cannot insert duplicate keys. */
      if(strcomp(temp->keystr, key) == 0) {
         shashlist_item_free(&item);
         return(NULL);
      }
      /* Fix up the case where we are appending instead of inserting. */
      if(temp == list->tail) {
         list->tail = item;
      } else {
         temp->next->prev = item;
         item->next = temp->next;
      }
      temp->next = item;
      item->prev = temp;
   }

   /* Add the def to the hash chains */
   hashindex = shash_string(list->hashbits, key);
   if(list->hash[hashindex] == NULL) {
      /* Nothing on hash chain */
      list->hash[hashindex] = item;
   } else {
      /* Already an item on the hash chain */
      temp = list->hash[hashindex];
      while(temp->chain != NULL)
         temp = temp->chain;
      temp->chain = item;
   }

   /* Return the fully formed shashlist entry. */
   return(item);

}