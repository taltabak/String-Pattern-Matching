#include "slist.h"
#include <stdio.h>
#include <stdlib.h>

void slist_init(slist_t* l)
{
	if(l == NULL)   //case list is null
	{
		perror("Error: list is NULL , list did not initialized");
        return;
	}
    slist_head(l) = NULL;
    slist_tail(l) = NULL;
    slist_size(l) = 0;
}

void slist_destroy(slist_t* l,slist_destroy_t flag)
{
    if(l != NULL) //case list is NULL
    { 
        if (flag == SLIST_LEAVE_DATA)   //case data is not allocated
        {
            while(slist_head(l) != NULL)
            {
                slist_node_t* cur_node = slist_head(l);
                slist_head(l) = slist_next(slist_head(l));
                free(cur_node);
            }
			slist_size(l) = 0;  
        }
        else if (flag == SLIST_FREE_DATA)    //case data is not allocated 
        {
            while(slist_head(l) != NULL)
            {
                slist_node_t* cur_node = slist_head(l);
                slist_head(l) = slist_next(slist_head(l));
                free(slist_data(cur_node));
                free(cur_node);
            }
			slist_size(l) = 0;  
        }
		else
		{
			perror("Error: invalid flag sent to slist_destroy , the function did not execute.");
		}    
    }
}

void* slist_pop_first(slist_t* l)
{
    if(l == NULL)   //case list is null
	{
		perror("Error: list is NULL");
        return NULL;
	}
    if(slist_head(l) == NULL)   //case list is empty  
        return NULL;
    void* data = slist_data(slist_head(l));     //case list is not empty
    slist_node_t* cur_node = slist_head(l);
    slist_head(l) = slist_next(slist_head(l));
    free(cur_node);
    slist_size(l)--;
    return data;
}

int slist_append(slist_t* l ,void* data)
{
	if(l == NULL)
	{
        perror("Error: list is NULL");
        return -1;
    }
    slist_node_t* cur_node = (slist_node_t*)malloc(sizeof(slist_node_t));
    if (cur_node == NULL)   //case malloc failed
    {
        perror("Error: malloc failed (node)");
        return -1;
    }
    slist_data(cur_node) = data;
    slist_next(cur_node) = NULL;
    if(slist_size(l) == 0)    //case list is empty
    {
        slist_head(l) = cur_node;
        slist_tail(l) = slist_head(l);
    }
    else    //case list is not empty
    {
        slist_next(slist_tail(l)) = cur_node;
        slist_tail(l) = cur_node;
    }
    slist_size(l)++;
    return 0;
}

int slist_prepend(slist_t* l ,void* data)
{
	if(l == NULL)
	{
        perror("Error: list is NULL");
        return -1;
    }
    slist_node_t* cur_node = (slist_node_t*)malloc(sizeof(slist_node_t));
    if (cur_node == NULL)   //case malloc failed
    {
        perror("Error: malloc failed (node)");
        return -1;
    }
    slist_data(cur_node) = data;
    if(slist_size(l) == 0)    //case list is empty
    {
        slist_next(cur_node) = NULL;
        slist_head(l) = cur_node;
        slist_tail(l) = slist_head(l);
    }
    else    //case list is not empty
    {
        slist_next(cur_node) = slist_head(l);
        slist_head(l) = cur_node;
    }
    slist_size(l)++;
    return 0;
}

int slist_append_list(slist_t* l_d, slist_t* l_s)
{
    if(l_d == NULL)
    {
		perror("Error: destination list is NULL");
        return -1;
    }
    slist_node_t* cur_node = slist_head(l_s);
    for(int i = 0 ; i < slist_size(l_s) ; i++)
    {
        int i = slist_append(l_d,slist_data(cur_node));
        if(i == -1)     //case malloc failed
            return -1;
        cur_node = slist_next(cur_node);
    }
    return 0;
}
