#include <stdio.h>
#include <stdlib.h>
#include "slist.h"
#include "pattern_matching.h"

int main ()
{
    pm_t* pm = (pm_t*)malloc(sizeof(pm_t));
    pm_init(pm);
    /*pm_addstring(pm,"be",7);
    pm_addstring(pm,"eb",2);
    pm_addstring(pm,"bebe",4);
    pm_addstring(pm,"ee",2);
    pm_addstring(pm,"bb",2);
    pm_addstring(pm,"ebeb",4);*/
    pm_addstring(pm,"a",1);
    pm_addstring(pm,"abc",3);
    pm_addstring(pm,"bca",3);
    pm_addstring(pm,"cab",3);
    pm_addstring(pm,"acb",3);
    pm_makeFSM(pm);
    slist_t* matches = pm_fsm_search(pm->zerostate,"xyzabcabde" ,10 );
    int size = slist_size(matches);
    slist_node_t* node = slist_head(matches);
    for(int i =0 ; i<size ; i++)
    {
        free(((pm_match_t*)slist_data(node))->pattern);
        node = slist_next(node);
    }
    slist_destroy(matches,SLIST_FREE_DATA);
    free(matches);
    pm_destroy(pm);
    free(pm);
    /*slist_t* list = (slist_t*)malloc(sizeof(slist_t));
    slist_init(list);
    slist_append(NULL, "asdad");
    slist_destroy(list,0);
    return 0;*/
    
}

