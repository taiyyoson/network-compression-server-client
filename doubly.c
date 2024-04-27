/* doubly_linked_list.c
 *
 * Implement a sorted linked list of strings with operations Insert
 * in alphabetical order, Print, Member, Delete, Free_list.
 * The list nodes are doubly linked.
 *
 * Input:    Single character lower case letters to indicate operations,
 *           possibly followed by value needed by operation -- e.g. 'i'
 *           followed by "hello" to insert the string "hello" -- no
 *           double or single quotes.
 *
 * Output:   Results of operations.
 *
 * Compile:  gcc -g -Wall -o dllist doubly_linked_list.c
 *
 * Run:      ./dllist
 *
 */

/* You may not add any new header file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Max 99 chars + '\0' */
#define STRING_MAX 100

struct list_node_s {
   char*  data;
   struct list_node_s* prev;
   struct list_node_s* next;
};

/* Pointers to the head and tail of the list */
struct list_s {
   struct list_node_s* h_p;
   struct list_node_s* t_p;
};

void Insert(struct list_s* list_p, char string[]);
void Print(struct list_s* list_p);
int  Member(struct list_s* list_p, char string[]);
void Delete(struct list_s* list_p, char string[]);
void Free_list(struct list_s* list_p);
char Get_command(void);
void Get_string(char string[]);
void Free_node(struct list_node_s* node_p);
struct list_node_s* Allocate_node(int size);
void Print_node(char title[], struct list_node_s* node_p);


/*-----------------------------------------------------------------*/
int main(void) {
   char          command;
   char          string[STRING_MAX];
   struct list_s list;

   list.h_p = list.t_p = NULL;
      /* start with empty list */

   command = Get_command();
   while (command != 'q' && command != 'Q') {
      switch (command) {
         case 'i':
         case 'I':
            Get_string(string);
            Insert(&list, string);
            break;
         case 'p':
         case 'P':
            Print(&list);
            break;
         case 'm':
         case 'M':
            Get_string(string);
            if (Member(&list, string))
               printf("%s is in the list\n", string);
            else
               printf("%s is not in the list\n", string);
            break;
         case 'd':
         case 'D':
            Get_string(string);
            Delete(&list, string);
            break;
         case 'f':
         case 'F':
            Free_list(&list);
            break;
         default:
            printf("There is no %c command\n", command);
            printf("Please try again\n");
      }
      command = Get_command();
   }
   Free_list(&list);

   return 0;
}  /* main */


/*-----------------------------------------------------------------*/
/* Function:   Allocate_node
 * Purpose:    Allocate storage for a list node
 * Input arg:  size = number of chars needed in data member (including
 *                storage for the terminating null)
 * Return val: Pointer to the new node
 */
struct list_node_s* Allocate_node(int size) {

	//allocate memory for new node
	struct list_node_s *newNode = (struct list_node_s*) malloc(sizeof(struct list_node_s));
	//check if there's enough space
	if (newNode == NULL) {
		printf("Unable to allocate memory for node");
		return NULL;
	}
	//initialize struct members
	newNode->data = (char*) malloc((size+1)*sizeof(char));
	newNode->next = NULL;
	newNode->prev = NULL;
	return newNode;

}  /* Allocate_node */


/*-----------------------------------------------------------------*/
/* Function:   Insert
 * Purpose:    Insert new node in correct alphabetical location in list
 * Input arg:  string = new string to be added to list
 * In/out arg: list_p = pointer to struct storing head and tail ptrs
 * Note:       If the string is already in the list, print a message
 *                and return, leaving list unchanged
 */
void Insert(struct list_s* list_p, char string[]) {

	//this requires trav pointer, set equal to head then use that
	struct list_node_s *trav;

	//create new node for string
	struct list_node_s *newNode = Allocate_node(strlen(string));
	strcpy(newNode->data, string);

	//if list hasn't been created yet
	if (list_p->h_p == NULL) {
		//set head and tail pointers
		list_p->h_p = newNode;
		list_p->t_p = newNode;
		return;
	}

	else {
		//set trav to head
		trav = list_p->h_p;

		//increment through linked list, find place to insert newNode
		while (trav != NULL && trav->next != NULL) {
			//set tail pointer to trav, trav is latest node
			list_p->t_p = trav;

			//second case, if first non-matching char of newNode is less than first non-matching char of trav node
			if (strcmp((newNode->data), (trav->data)) < 0 ) {
				//if trav is currently head node
				if ((trav->prev) == NULL) {
					//set new head pointer to newNode
					list_p->h_p = newNode;
					newNode->next = trav;
					trav->prev = newNode;
					return;
				}
				//if not head node, set pointers to make space for newNode, prev and next pointers now point to newNode
				newNode->next = trav;
				newNode->prev = trav->prev;
				trav->prev->next = newNode;
				trav->prev = newNode;
				return;
			}

			//third case, if strings are equal, don't change list, return
			else if (strcmp((newNode->data), (trav->data)) == 0) {
				printf("String already stored in list\n");
				Free_node(newNode);
				return;
			}


			//increment trav node, means newNode alphabetically is greater than current trav node
			trav = trav->next;
		}
		//if trav->next == NULL, create new pointer at end of list
		newNode->prev = trav;
		trav->next = newNode;
		list_p->t_p = newNode;
	}

}  /* Insert */


/*-----------------------------------------------------------------*/
/* Function:   Print
 * Purpose:    Print the contents of the nodes in the list
 * Input arg:  list_p = pointers to first and last nodes in list
 *
 * Hint: The implementation of this function (below) shows how
 * to traverse a linked list.
 */
void Print(struct list_s* list_p) {
   struct list_node_s* curr_p = list_p->h_p;

   printf("list = ");

   while (curr_p != NULL) {
      printf("%s ", curr_p->data);
      curr_p = curr_p->next;
   }
   printf("\n");
}  /* Print */


/*-----------------------------------------------------------------*/
/* Function:   Member
 * Purpose:    Search list for string
 * Input args: string = string to search for
 *             list_p = pointers to first and last nodes in list
 * Return val: 1, if string is in the list, 0 otherwise
 */
int Member(struct list_s* list_p, char string[]) {

	//if list is empty
	if (list_p->h_p == NULL)
		return 0;
	//trav pointer
	struct list_node_s *trav = list_p->h_p;
	while (trav != NULL) {
		//if same, then true
		if (strcmp((trav->data), string) == 0)
			return 1;
		//for efficiency, means trav->data is greater alphabetically, so no need to go through rest of list
		if (strcmp((trav->data), string) > 0)
			break;
		trav = trav->next;
	}

  	return 0;
}  /* Member */


/*-----------------------------------------------------------------*/
/* Function:   Free_node
 * Purpose:    Free storage used by a node of the list
 * In/out arg: node_p = pointer to node to be freed
 */
void Free_node(struct list_node_s* node_p) {
   free(node_p->data);
   free(node_p);
}  /* Free_node */


/*-----------------------------------------------------------------*/
/* Function:   Delete
 * Purpose:    Delete node containing string.
 * Input arg:  string = string to be deleted
 * In/out arg  list_p = pointers to head and tail of list
 * Note:       If the string is in the list, it will be unique.  So
 *             at most one node will be deleted.  If the string isn't
 *             in the list, the function just prints a message and
 *             returns, leaving the list unchanged.
 */
void Delete(struct list_s* list_p, char string[]) {
	//set trav pointer
	struct list_node_s *trav = list_p->h_p;
	//if list is empty, can't delete
	if (trav == NULL) {
		printf("List empty\n");
		return;
	}

	//if head node is subject for deletion
	if (strcmp((trav->data), string) == 0) {
		//if only one node in list
		if (trav->next == NULL)
			list_p->h_p = NULL;
		//if more than one node in list
		else
			list_p->h_p = trav->next;
		list_p->h_p->prev = NULL;
		Free_node (trav);
		return;
	}


	while (trav != NULL) {
		//if matched node for deletion
		if (strcmp((trav->data), string) == 0) {
			//if tail node
			if (trav->next == NULL) {
				trav->prev->next = NULL;
				list_p->t_p = trav->prev;
			}
			//not tail node
			else {
				trav->prev->next = trav->next;
				trav->next->prev = trav->prev;
			}

			Free_node(trav);
			return;
		}
		//efficient memory saving, break if we're already past same letters, no need to loop through the rest of the list
		if (strcmp((trav->data), string) > 0) {
			break;
		}

		trav = trav->next;
	}
	//message for string not found in list
	printf("String not in list\n");

}  /* Delete */

/*-----------------------------------------------------------------*/
/* Function:   Free_list
 * Purpose:    Free storage used by list
 * In/out arg: list_p = pointers to head and tail of list
 */
void Free_list(struct list_s* list_p) {
	//trav and temp pointers
	struct list_node_s *trav = list_p->h_p;
	struct list_node_s *temp;
	while (trav != NULL) {
		temp = trav;
		trav = trav->next;
		//remove/free each node then go to next
		Free_node(temp);
	}

}  /* Free_list */


/*-----------------------------------------------------------------*/
/* Function:   Get_command
 * Purpose:    Find and return the next non-whitespace character in
 *             the input stream
 * Return val: The next non-whitespace character in stdin
 */
char Get_command(void) {
   char c;

   printf("Please enter a command (i, d, m, p, f, q):  ");
   /* Put the space before the %c so scanf will skip white space */
   scanf(" %c", &c);
   return c;
}  /* Get_command */

/*-----------------------------------------------------------------*/
/* Function:   Get_string
 * Purpose:    Read the next string in stdin (delimited by whitespace)
 * Out arg:    string = next string in stdin
 */
void Get_string(char string[]) {

   printf("Please enter a string:  ");
   scanf("%s", string);
}  /* Get_string */


/*-----------------------------------------------------------------*/
/* Function:  Print_node
 * Purpose:   Print the data member in a node or NULL if the
 *            pointer is NULL
 * In args:   title:  name of the node
 *            node_p:  pointer to node
 */
void Print_node(char title[], struct list_node_s* node_p) {
   printf("%s = ", title);
   if (node_p != NULL)
      printf("%s\n", node_p->data);
   else
      printf("NULL\n");
}  /* Print_node */


