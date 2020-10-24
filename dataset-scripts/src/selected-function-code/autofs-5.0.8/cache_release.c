void cache_release(struct map_source *map)
{
	struct mapent_cache *mc;
	struct mapent *me, *next;
	int status;
	unsigned int i;

	mc = map->mc;

	cache_writelock(mc);

	for (i = 0; i < mc->size; i++) {
		me = mc->hash[i];
		if (me == NULL)
			continue;
		next = me->next;
		free(me->key);
		if (me->mapent)
			free(me->mapent);
		free(me);

		while (next != NULL) {
			me = next;
			next = me->next;
			free(me->key);
			if (me->mapent)
				free(me->mapent);
			free(me);
		}
	}

	map->mc = NULL;

	cache_unlock(mc);

	status = pthread_mutex_destroy(&mc->ino_index_mutex);
	if (status)
		fatal(status);

	status = pthread_rwlock_destroy(&mc->rwlock);
	if (status)
		fatal(status);

	free(mc->hash);
	free(mc->ino_index);
	free(mc);
}