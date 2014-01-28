#ifndef MISC_H
#define MISC_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h> /* for memcpy() */

/*********************Debugging Macros********************
 * wiki.tox.im/index.php/Internal_functions_and_data_structures#Debugging
 *********************************************************/
#ifdef DEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define DEBUG_PRINT(str, ...) do { \
        char msg[1000]; \
        sprintf(msg, "%s(): line %d (file %s): %s%%c\n", __FUNCTION__, __LINE__, __FILE__, str); \
        fprintf(stderr, msg, __VA_ARGS__); \
    } while (0)

#define WARNING(...) do { \
        fprintf(stderr, "warning in "); \
        DEBUG_PRINT(__VA_ARGS__, ' '); \
    } while (0)

#define INFO(...) do { \
        DEBUG_PRINT(__VA_ARGS__, ' '); \
    } while (0)

#undef ERROR
#define ERROR(exit_status, ...) do { \
        fprintf(stderr, "error in "); \
        DEBUG_PRINT(__VA_ARGS__, ' '); \
        exit(exit_status); \
    } while (0)
#else
#define WARNING(...)
#define INFO(...)
#undef ERROR
#define ERROR(...)
#endif // DEBUG

/************************Linked List***********************
 * http://wiki.tox.im/index.php/Internal_functions_and_data_structures#Linked_List
 * TODO: Update wiki.
 **********************************************************/

/* Example usage

This sample program makes a new struct which contains a
character and a tox_list_t. It then prompts a user for
input until he enters q or e. It then adds each character
to the list, and uses a special for loop to print them.
It then removes all the 'z' characters, and prints the list
again.

//Notice that the data to be put in the list *contains* tox_list_t;
//usually, this is the other way around!
typedef struct tox_string {
   char c;
   tox_list_t tox_lst; //Notice that tox_lst is *NOT* a pointer.
} tox_string_t;

int main()
{
   tox_list_t head;
   tox_list_new(&head); //initialize head

   //input a new character, until user enters q or e
   char c = '\0';
   while (c != 'q' && c != 'e') {
      scanf("%c", &c);
      tox_string_t* tmp = malloc(sizeof(tox_string_t));
      tmp->c = c;
      tox_list_add(&head, &tmp->tox_lst); //add it to the list
   }

TOX_LIST_FOR_EACH() takes a struct tox_list and a name for a temporary pointer to use in the loop.

TOX_LIST_GET_VALUE() uses magic to return an instance of a structure that contains tox_list_t.
You have to give it a temporary tox_string_t, name of tox_list_t member inside our structure (tox_lst),
and the type of structure to return.

   TOX_LIST_FOR_EACH(head, tmp)
      printf("%c", TOX_LIST_GET_VALUE(*tmp, tox_lst, tox_string_t).c);

   TOX_LIST_FOR_EACH(head, tmp) {
      if (TOX_LIST_GET_VALUE(*tmp, tox_lst, tox_string_t).c == 'z') {
         //If you delete tmp, you have to quit the loop, or it will go on infinitly.
         //This will be fixed later on.
         tox_list_remove(tmp);
         break;
      }
   }

   printf("\n");
   TOX_LIST_FOR_EACH(head, tmp)
      printf("%c", TOX_LIST_GET_VALUE(*tmp, tox_lst, tox_string_t).c);


   return 0;
}
*/

#define MEMBER_OFFSET(var_name_in_parent, parent_type) \
   (&(((parent_type*)0)->var_name_in_parent))

#define GET_PARENT(var, var_name_in_parent, parent_type) \
   ((parent_type*)((uint64_t)(&(var)) - (uint64_t)(MEMBER_OFFSET(var_name_in_parent, parent_type))))

#define TOX_LIST_FOR_EACH(lst, tmp_name) \
   for (tox_list* tmp_name = lst.next; tmp_name != &lst; tmp_name = tmp_name->next)

#define TOX_LIST_GET_VALUE(tmp_name, name_in_parent, parent_type) GET_PARENT(tmp_name, name_in_parent, parent_type)

typedef struct tox_list {
    struct tox_list *prev, *next;
} tox_list;

/*  return new tox_list_t. */
static inline void tox_list_new(tox_list *lst)
{
    lst->prev = lst->next = lst;
}

/* Inserts a new tox_lst after lst and returns it. */
static inline void tox_list_add(tox_list *lst, tox_list *new_lst)
{
    tox_list_new(new_lst);

    new_lst->next = lst->next;
    new_lst->next->prev = new_lst;

    lst->next = new_lst;
    new_lst->prev = lst;
}

static inline void tox_list_remove(tox_list *lst)
{
    lst->prev->next = lst->next;
    lst->next->prev = lst->prev;
}

#endif
