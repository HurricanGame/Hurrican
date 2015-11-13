#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

//DKS - This is a new header I've added to Hurrican in the process of optimizing it.
//      It introduces custom data structures I created:
//
//      Class MemPool is a very simple and fast pooled memory manager that can hold up to
//      32767 objects. It was designed for use with the particle and projectile systems,
//      to eliminate the hefty overhead of calling new/delete/malloc/free for each
//      individual projectile/particle.

template <class OBJECT_TYPE, int MAX_OBJECTS>
class MemPool
{
    public:
        MemPool()  { reinit(); }
        ~MemPool() {}

        OBJECT_TYPE *alloc()
        {
#ifdef DEBUG_MEMPOOL
            if (head_of_free_list == -1) {
                Protokoll.WriteText( false, "ERROR in MemPool, tried to allocate more than %d objects\n", MAX_OBJECTS );
                return NULL;
            }
#endif // DEBUG_MEMPOOL

            OBJECT_TYPE *p = &pool[head_of_free_list];
            head_of_free_list = free_list[head_of_free_list];
            return p;
        }

        void free(OBJECT_TYPE *p)
        {

            /* If, for whatever reason, your platform lacks the types ptrdiff_t or uintptr_t, you can use this:
#if (defined(ARCHITECTURE_32BIT) && !defined(__x86_64__) && !defined(__LP64__) && !defined (__amd64__) \
&& !defined(__aarch64__))
int16_t idx = (int16_t)(((uint32_t)p - (uint32_t)pool) / sizeof(OBJECT_TYPE));
#else 
int16_t idx = (int16_t)(((uint32_t)((uint64_t)p - (uint64_t)pool)) / sizeof(OBJECT_TYPE));
#endif
             */
            int16_t idx = (int16_t)(((ptrdiff_t)((uintptr_t)p - (uintptr_t)pool)) / sizeof(OBJECT_TYPE));
            free_list[idx] = head_of_free_list;
            head_of_free_list = idx;
        }

        // reinit() can be called once the game is done with a level. This reinitializes the
        // free list to a pristine state so there is maximum cache locality and prefetching.
        void reinit()
        {
            for (int i=0; i < MAX_OBJECTS-1; ++i) {
                free_list[i] = i+1;  // This entry points to next entry in free list array
            }

            free_list[MAX_OBJECTS-1] = -1; // Terminate end of free list
            head_of_free_list = 0;
        }

    private:
        OBJECT_TYPE pool[MAX_OBJECTS];
        int16_t head_of_free_list;
        int16_t free_list[MAX_OBJECTS];
};

#endif // DATASTRUCTURES_H
