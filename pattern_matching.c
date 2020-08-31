#include "pattern_matching.h"
#include "slist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Auxiliary function:
/**Finds the failure states of a state's sons, given the paramters:
 FSM , 
 state with an existing failure state , 
 lists of the edges of the state **/
void findfailure (pm_t* , pm_state_t* , slist_t* );

int pm_init(pm_t * p)
{
    if(p ==NULL)
    {
        perror("Error: the pm is NULL");
        return -1;
    }
    p->zerostate = (pm_state_t*)malloc(sizeof(pm_state_t));
    if(p->zerostate == NULL)    //case malloc failed
    {
        perror("Error: malloc failed (zerostate)");
        return -1;
    }
    p->zerostate->id = 0;
    p->zerostate ->depth = 0;
    p->zerostate ->output = NULL;
    p->zerostate ->_transitions = (slist_t*)malloc(sizeof(slist_t));
    if(p->zerostate ->_transitions == NULL) //case malloc failed
    {
        free(p->zerostate);
        perror("Error: malloc failed (edges)");
        return -1;
    }
    slist_init(p->zerostate ->_transitions);
    p->zerostate ->fail = NULL;
    p->newstate = 1;
    return 0;
}


int pm_goto_set(pm_state_t* from_state , unsigned char symbol , pm_state_t* to_state)
{
    if(from_state == NULL || to_state == NULL)  //case one of the parameters is NULL
    {
        perror("Error: one of the states or more is NULL");
        return -1;
    }
    pm_labeled_edge_t* edge = (pm_labeled_edge_t*)malloc(sizeof(pm_labeled_edge_t));
    if(edge == NULL)    //case malloc failed
    {
        perror("Error: malloc failed (edge)");
        return -1;
    }
    edge->label = symbol;
    edge->state = to_state;
    if(slist_append(from_state->_transitions,edge) == -1)   
    {
        free(edge);
        return -1;  //case append failed 
    }
    return 0;
}


pm_state_t* pm_goto_get(pm_state_t* state, unsigned char symbol)
{
    if(state == NULL)   //case one of the parameters is NULL
    {
        perror("Error: state is NULL");
        return NULL;
    }
    slist_node_t* cur_state = slist_head(state->_transitions); 
    while(cur_state != NULL)    //checking all edges and compare the symbols
    {
        if (((pm_labeled_edge_t*)slist_data(cur_state))->label == symbol)   
            return ((pm_labeled_edge_t*)slist_data(cur_state))->state;  //return the state
        cur_state=slist_next(cur_state);
    }
    return NULL;    //case the is no accordance edge 
}

int pm_addstring(pm_t* p ,unsigned char* string , size_t n)
{
    if(p ==NULL)
    {
        perror("Error: the pm is NULL the string was not added to the fsm" );
        return -1;
    }
    if(string == NULL)
    {
        perror("Error: invalid string (NULL), the string was not added to the fsm");
        return -1;
    }
    if(strlen(string) != n)
    {
        perror("Error: invalid input (size does not equal to string length size) the string was not added to the fsm");
        return -1;
    }
    pm_state_t* state = p->zerostate;
    for(int i = 0 ; i < n ; i++)    //for each letter in the string
    {
        pm_state_t* newstate = pm_goto_get(state,string[i]);
        if (newstate == NULL)   //case the new state does not exist yet (create a new state).
        {
            newstate = (pm_state_t*)malloc(sizeof(pm_state_t));
            if(newstate == NULL)    //case malloc failed
            {
                perror("Error: malloc failed (newstate)");
                return -1;
            }
            newstate->_transitions = (slist_t*)malloc(sizeof(slist_t));   
            if(newstate ->_transitions == NULL) //case malloc failed
            {
                free(newstate);
                perror("Error: malloc failed (edges)");
                return -1;
            }
            slist_init(newstate ->_transitions);
            newstate->depth = state->depth +1;
            newstate->id = p->newstate;
            newstate->output = (slist_t*)malloc(sizeof(slist_t));
            if(newstate->output == NULL) //case malloc failed
            {
                free(newstate);
                free(newstate->_transitions);
                perror("Error: malloc failed (outputs)");
                return -1;
            }
            slist_init(newstate->output);
            p->newstate++;
            if(pm_goto_set(state,string[i],newstate) == -1)
            {
                free(newstate);
                free(newstate->_transitions);
                free(newstate->output);
                return -1;  //case goto_set failed
            }
            printf("Allocating state %d\n",newstate->id);   //print each time newstate created.
            printf("%d -> %c -> %d\n",state->id,string[i],newstate->id);
            if(i == n-1)    //case there is need to update the output list (the last letter of the string).
                if(slist_append(newstate->output,string) == -1)
                {
                free(newstate);
                free(newstate->_transitions);
                free(newstate->output);
                return -1;  //case append failed 
            }
        }
        state = newstate; //moving to the next state
    }
}


int pm_makeFSM(pm_t* p)
{
    if(p ==NULL)
    {
        perror("Error: the pm is NULL (make_fsm");
        return -1;
    }
    p->zerostate->fail = p->zerostate;
    slist_t* queue = (slist_t*)malloc(sizeof(slist_t)); //queue to hold states by depth
    if(queue == NULL) //case malloc failed
    {
        perror("Error: malloc failed (queue)");
        return -1;
    }
    slist_init(queue);
    if(slist_append_list(queue,p->zerostate->_transitions)== -1)   
    { 
        free(queue);
        return -1;  //case append failed
    }
    slist_node_t* cur_edge = slist_head(queue);
    while(cur_edge != NULL) //update all zerostate's sons's failue state to be zerostate. 
    {
        ((pm_labeled_edge_t*)slist_data(cur_edge))->state->fail = p->zerostate;
        cur_edge = slist_next(cur_edge);
    }
    while(slist_size(queue) != 0)   //for each edge in queue update the failue state of the 'to_state' based on 'from_state' failure state.
    {
        pm_labeled_edge_t* edge = slist_pop_first(queue);
        pm_state_t* state = edge->state;
        findfailure(p , state , state->_transitions); 
        printf("Setting f(%d) = %d\n",state->id,state->fail->id); //print each time failue state is setted
        if(slist_append_list(queue,edge->state->_transitions) == -1)    
        { 
            free(queue);
            return -1;  //case append failed
        }
    }
    free (queue);
}

void findfailure (pm_t* p , pm_state_t* state, slist_t* edges)
{
    slist_node_t* cur_edge = slist_head(edges);
    while(cur_edge != NULL)   //for each edge update all it's 'to_state's' failue state. 
    {
        pm_state_t* sonstate = ((pm_labeled_edge_t*)slist_data(cur_edge))->state;
        unsigned char symbol = ((pm_labeled_edge_t*)slist_data(cur_edge))->label;
        pm_state_t* fathefailure = state->fail;
        while(1)    //determine failue state
        {
            pm_state_t* possiblefail = pm_goto_get(fathefailure,symbol);
            if(possiblefail != NULL)    //case goto_get succeed
            {
                if(slist_append_list(sonstate->output,possiblefail->output)==-1)
                    return; //case append failed 
                sonstate->fail = possiblefail;
                break;
            }
            else    //case goto_get failed
            {
                if(fathefailure->id == 0) //case the failure state is of father is zerostate.
                {
                    sonstate->fail = p->zerostate;
                    break;
                }
                fathefailure = fathefailure->fail;  //continue to check ancestor's failure state. 
                continue;
            }
        }
        cur_edge = slist_next(cur_edge);
    }
}

slist_t* pm_fsm_search(pm_state_t* zerostate , unsigned char* string , size_t n)
{
    if(zerostate ==NULL)
    {
        perror("Error: the zerostate is NULL, the search function did not execute");
        return NULL;
    }
    if(string == NULL)
    {
        perror("Error: invalid string (NULL), the search function did not execute");
        return NULL;
    }
    if(strlen(string) != n)
    {
        perror("Error: invalid input (size does not equal to string length size , the search function did not execute)");
        return NULL;
    }
    slist_t* matches= (slist_t*)malloc(sizeof(slist_t));    
    if(matches == NULL) //case malloc failed
    {
        perror("Error: malloc failed (matches)");
        return NULL;
    }
    slist_init(matches);
    pm_state_t* state = zerostate;
    for(int i=0 ; i<n ; i++) //for each letter in the string
    {
        pm_state_t* nextstate = pm_goto_get(state,string[i]);
        if(nextstate == NULL)   //goto_get failed and there is need to check the failure state.
        {
            if(state->id != 0)
                {
                i--;    //moving to failure state does not take out letter 
                state = state->fail;
                }
        }
        else    //case goto_get succeeded
        {
            state = nextstate;
            slist_node_t* output = slist_head(state->output);
            while(output != NULL)   //for each output in the current state's output list add a match.
            {
                unsigned int length= strlen((char*)slist_data(output));
                char* pattern = (char*)malloc((length+1)*sizeof(char));
                if(pattern == NULL) //case malloc failed
                {
                    free(matches);
                    perror("Error: malloc failed (pattern)");
                    return NULL;
                }
                strcpy(pattern,(char*)slist_data(output));
                output = slist_next(output);
                pm_match_t* match= (pm_match_t*)malloc(sizeof(pm_match_t));
                if(match == NULL) //case malloc failed
                {
                    free(matches);
                    perror("Error: malloc failed (match)");
                    return NULL;
                }
                match->pattern = pattern;
                match->fstate = state;
                match->start_pos = i-(length-1);
                match->end_pos = i;
                printf("Pattern: %s, starts at: %d, ends at %d, last state: %d\n", match->pattern,match->start_pos,match->end_pos,match->fstate->id);
                //^^print each time match has found.^^
                if(slist_append(matches,match)== -1)
                {
                    free(matches);
                    return NULL;    //case append failed
                } 
            }
        }   
    }
    return matches;
}

void pm_destroy(pm_t *p)
{
    if(p == NULL)
        return;
    slist_t* queue = (slist_t*)malloc(sizeof(slist_t)); //queue to hold all states by depth
    if(queue == NULL)    //case malloc failed
    {
        perror("Error: malloc failed (queue)");
        return;
    }
    slist_init(queue);
    if(slist_append(queue,p->zerostate) == -1)
    {   
        free(queue);
        return; //case append failed 
    }
    while(slist_size(queue) != 0) //for each state in the queue:
    {
        pm_state_t* state = slist_pop_first(queue);
        unsigned int sizeofedges = slist_size(state->_transitions);
        for(int i = 0 ; i < sizeofedges ; i++)  //for each edge of the state append all states to queue and free the edge.
        {
            pm_labeled_edge_t* edge = slist_pop_first(state->_transitions);
            if(slist_append(queue,edge->state) == -1)
                return; //case append failed 
            free(edge);
        }
        slist_destroy(state->output, SLIST_LEAVE_DATA);
        free(state->_transitions);
        free(state->output);
        free(state);
    }
    free(queue);
}