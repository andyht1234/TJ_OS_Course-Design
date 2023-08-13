//main()
int main()
{
    for (int i = 0; i < NBUCKET; i++) 
    {
        pthread_mutex_init(&locks[i], NULL);
    }
}

//put()
static 
void put(int key, int value)
{
  int i = key % NBUCKET;
  pthread_mutex_lock(&locks[i]);  //加锁
  // is the key already present?
  struct entry *e = 0;
  for (e = table[i]; e != 0; e = e->next) {
    if (e->key == key)
      break;
  }
  if(e){
    // update the existing key.
    e->value = value;
  } else {
    // the new is new.
    insert(key, value, &table[i], table[i]);
  }
  pthread_mutex_unlock(&locks[i]); //放锁
}

//get()
static struct entry*
get(int key)
{
  int i = key % NBUCKET;
  pthread_mutex_lock(&locks[i]);  //加锁
  struct entry *e = 0;
  for (e = table[i]; e != 0; e = e->next) {
    if (e->key == key) break;
  }
  pthread_mutex_unlock(&locks[i]); //放锁
  return e;
}