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
                Protokoll.WriteText( false, "File: %s Line: %d\n", __FILE__, __LINE__ );
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

/***
   GroupedForwardList class is a write-once read-many type of data structure. It is singly-linked.

DESCRIPTION:

   Class 'GroupedForwardList' is a singly-linked statically-allocated grouping list,
   designed for use in the tile engine, projectile system, and particle systems in this game,
   grouping draw-calls that use similar textures together so the most efficient
   batching of draw-calls can occur.

   Items inserted into the class must have the following two variables in their structure/class:
   (UINT16 OR UINT8) grouped_list_key;      // The key value must never be negative!
   (INT16)  grouped_list_next_node_idx;     // This is for the GroupedList classes's internal use, and
                                            // MUST be a signed int
 
   Items read back from the GroupedForwardList are coalesced according to their key. Not necessarily in any order,
   but always grouped all together by the same key.  If a user wishes to read from the start of the array
   after having read from it once before, they must call begin_get_items() first.

INTERNAL WORKINGS:
   All the data is stored in a statically-allocated array list[] so there's no allocation overhead.
   There are two additional arrays for bookkeeping:

      struct ListMapEntry {
         int16_t head_node_idx;
         int16_t cur_node_idx;
      };

      ListMapEntry list_map[MAX_LIST_GROUPS];

   This array, list_map, works together with the below array:

      LIST_MAP_INDEX_INT_TYPE list_map_map[MAX_LIST_GROUPS];

   An incoming key value, item.grouped_list_key, is used to index into list_map_map[].
   From list_map_map, it gets the correct index into list_map[] for that group.
   From list_map[] it gets the two pieces of info it needs to either store or access
      information in list[] associated with that group.
      
   list_map[ list_map_map[(key_value)] ].head_node_idx is the index into list[] for the
         head-node of that group's data chain.

   list_map[ list_map_map[(key_value)] ].cur_node_idx is the index into list[] for the
         current tail-node of that group's data chain.

TEMPLATE PARAMETERS:
   LIST_ITEM_TYPE is the type of items this list contains.  Member items need to contain two specific
      variables, described in the DESCRIPTION section above.
   LIST_MAP_INDEX_INT_TYPE is the type of integer of our list_map[] array. Needs to be a signed integer,
       and able to hold up to NUM_LIST_GROUPS-1. So, for example,  int8_t is OK if the number of groups does
       not exceed 128 (0-127). int16_t should be used if there are more groups than 127.
   MAX_LIST_GROUPS is the total number of groups this GroupedForwardList is going to be asked to group
   MAX_LIST_ITEMS is the size of the internal array of LIST_ITEM_TYPEs

MEMBER FUNCTIONS:
   void clear():            
      Clear the structure so that all-new data can be inserted. 

   int size():
      How many items are stored in the list?

   int get_num_groups():
      How many different groups are in the list?

   ITEM_TYPE* data(): 
      A direct pointer to the front of the contiguous internal list of ITEM_TYPEs. 
      If the user calls get_num_groups(), they can easily tell if there is just one group and
      can instead call this function to get a direct pointer to the data and iterate on it faster.

   void push_back(&item): 
      Insert an item tracked by its key onto the end of the internal data

   void begin_get_items():
      Normally, if you insert data into the list, it can be read back from the beginning without 
      having to call this.  Call this if you need to read from the start multiple times in between
      writings.

   ITEM_TYPE* get_next_item():
      Return pointer to the next coalesced item, and return NULL if no more items.

   void begin_get_items_from_group(int key);
      If you would like to read all items from a specific group, call this with the group's key first.

   LIST_ITEM_TYPE* get_next_item_from_group();
      Return pointer to the next item from the current group, or NULL if there is no more items in the group.
      You must call begin_get_items_from_group(group_key) before calling this.

   void hide_group(int key);
      Call this to effectively remove a group from showing up in any subsequent reads (from the start)
***/
template <typename LIST_ITEM_TYPE, typename LIST_MAP_INDEX_INT_TYPE, int MAX_LIST_GROUPS, int MAX_LIST_ITEMS>
class GroupedForwardList {
   public:
      GroupedForwardList()  { clear(); }
      ~GroupedForwardList() {}

      void clear();

      int get_num_groups()   { return num_groups; }

      LIST_ITEM_TYPE* data() { return list; }

      void push_back(LIST_ITEM_TYPE &item);

      int size()             { return num_items; }

      void begin_get_items() 
      { 
         read_list_map_idx = 0;
         read_node_idx = list_map[0].head_node_idx;
      }

      LIST_ITEM_TYPE* get_next_item()
      {
         // Have we reached the end of the current group chain?
         // Or is this the head of a group chain that's been hidden?
         if (read_node_idx == -1 || read_node_idx == -2) {

            // Get head of next group chain, skipping past any groups that were hidden,
            //   having a list_map entry of -2.
            do {
               read_list_map_idx++;
               if (read_list_map_idx >= num_groups) {
                   // We've run through all possible group chains, return NULL, we're done..
                   return NULL;
               } 
               read_node_idx = list_map[read_list_map_idx].head_node_idx;
            } while (read_node_idx == -2);
         }

         if (read_node_idx == -1) {
            // We've come to the end of any possible additional entries
            return NULL;
         }

         LIST_ITEM_TYPE *tmp_ptr = &list[read_node_idx];
         read_node_idx = list[read_node_idx].grouped_list_next_node_idx;
         return tmp_ptr;
      }

      void begin_get_items_from_group(int key) 
      { 
         int list_map_idx = list_map_map[key]; 

         if (list_map_idx == -1) {
            // This group has no entries
            read_node_idx = -1;
         } else {
             read_node_idx = list_map[list_map_idx].head_node_idx;
             if (read_node_idx == -2) {
                 // Group has been hidden, don't read from it
                 read_node_idx = -1;
             }
         }
      }

      LIST_ITEM_TYPE* get_next_item_from_group()
      {
         if (read_node_idx == -1) {
            return NULL;
         }

         LIST_ITEM_TYPE *tmp_ptr = &list[read_node_idx];
         read_node_idx = list[read_node_idx].grouped_list_next_node_idx;
         return tmp_ptr;
      }

      void hide_group(int key)
      {
         int list_map_idx = list_map_map[key];

         if (list_map_map[key] != -1) {
            // Assign -2 value, indicating that any reads should skip this group
            list_map[list_map_idx].head_node_idx = -2;
         }
      }

#if 0
      // This code has been tested to work, but I don't need it in the game
      int count_items_in_group(int key) {
#ifdef _DEBUG
         if (key >= MAX_LIST_GROUPS) {
            cout << "ERROR: count_items_in_group() in GroupList, key: " << num_items 
               << " MAX_LIST_GROUPS: " << MAX_LIST_GROUPS << endl;
         }
#endif
         int ctr = 0;
         int list_map_idx = map_key_to_list_map(key);
         int cur_node_idx = list_map[list_map_idx].head_node_idx;

         while (cur_node_idx != -1) {
            cur_node_idx = list[cur_node_idx].grouped_list_next_node_idx;
            ++ctr;
         }

         return ctr;
      }
#endif //0
         
   private:
      LIST_ITEM_TYPE list[ MAX_LIST_ITEMS ];

      int num_items;
      int num_groups;
      int read_list_map_idx;
      int read_node_idx;

      struct ListMapEntry {
         int16_t head_node_idx;
         int16_t cur_node_idx;
      };

      ListMapEntry list_map[ MAX_LIST_GROUPS ];
      LIST_MAP_INDEX_INT_TYPE list_map_map[ MAX_LIST_GROUPS ];

      int map_key_to_list_map(int key) 
      {
         int list_map_idx = list_map_map[key]; 

         if (list_map_idx == -1) {
            // First time we've seen this key, so use the next unused list_map entry
            list_map_map[key] = list_map_idx = num_groups;
            num_groups++;
         } 

         return list_map_idx;
      }
};

template <typename LIST_ITEM_TYPE, typename LIST_MAP_INDEX_INT_TYPE, int MAX_LIST_GROUPS, int MAX_LIST_ITEMS>
void GroupedForwardList<LIST_ITEM_TYPE, LIST_MAP_INDEX_INT_TYPE, MAX_LIST_GROUPS, MAX_LIST_ITEMS>
::clear()
{
   num_items = 0;
   num_groups = 0;
   read_list_map_idx = 0;
   read_node_idx = 0;

   for (int i = 0; i < MAX_LIST_GROUPS; i++ ) {
      list_map_map[i] = -1;
      list_map[i].head_node_idx = -1;
   }
}

template <typename LIST_ITEM_TYPE, typename LIST_MAP_INDEX_INT_TYPE, int MAX_LIST_GROUPS, int MAX_LIST_ITEMS>
void GroupedForwardList<LIST_ITEM_TYPE, LIST_MAP_INDEX_INT_TYPE, MAX_LIST_GROUPS, MAX_LIST_ITEMS>
::push_back(LIST_ITEM_TYPE &item) 
{
#ifdef _DEBUG
   if (num_items >= MAX_LIST_ITEMS) {
       Protokoll.WriteText( false, "ERROR: push_back() in GroupList, num_items: %d MAX_LIST_ITEMS: %d\n", 
               num_items, MAX_LIST_ITEMS );
       Protokoll.WriteText( false, "File: %s Line: %d\n", __FILE__, __LINE__ );
   }
#endif
   int next_unused_slot_idx = num_items;
   list[next_unused_slot_idx] = item;
   list[next_unused_slot_idx].grouped_list_next_node_idx = -1;

   int list_map_idx = map_key_to_list_map( item.grouped_list_key );
   if (list_map[list_map_idx].head_node_idx == -1) {
      list_map[list_map_idx].head_node_idx = next_unused_slot_idx;
   } else {
      list[ list_map[list_map_idx].cur_node_idx ].grouped_list_next_node_idx = next_unused_slot_idx;
   }
   list_map[list_map_idx].cur_node_idx = next_unused_slot_idx;
   num_items++;
}

#endif // DATASTRUCTURES_H
