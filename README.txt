
CREATED BY: Tal Tabak.

Description: 
  String Pattern Matching Algorithm,
the algorithm first constructs DFA according to all strings user seeks to find, and then by running the main string on the DFA, the search for matches is in a running time of O(1). 

PROGRAM FILES: 
    1. slist.c -an implementaion of a simple linked list, the data stored in the nodes is generic. 
    2. pattern_matching.c - an implementaion of the pattern matching algorithm, (construction of the fsm and search functions).  

REMARKS:

*   Workspace: Visual Studio Code
*   Input: Strings to search for; A string to search inside. 
*   Output: FSM states, edges and failure states description ; The details of the pattern matches found in the string. 
*   Auxiliary functions: -findfailue- (in pattern_matching.c) . 
*   In order to run the file,the user will need to write a main in accordance with the program files.
    After the main implements the functions of the program, 
    the user will need to deallocate the memory allocated in the program.
    the function "pm_destroy" deallocates all the memory allocated by the fsm, 
    but the function "pm_fsm_search" will return a list of matches to the main and the user will have to 
    deallocate this list.  (an accordance code to deallocate the memory is attached in the bottom). 


ATTACHED CODE:
(Suggestion for a way to deallocate all the memory in the main)
    //****(matches is the list returned from search function)****// 
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
