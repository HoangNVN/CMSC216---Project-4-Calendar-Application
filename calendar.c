/*
 *Name: Hoang Nguyen
 *UID: 118426327
 *Directory ID: Hnguye38
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "calendar.h"

int init_calendar(const char *name, int days, 
                  int (*comp_func) (const void *ptr1, const void *ptr2),
                  void (*free_info_func) (void *ptr), Calendar **calendar) {
   int i = 0;
   if (calendar && name && days >= 1) {
      /*Allocate appropriate fields*/
      /*Calendar **new_calendar = malloc(sizeof(calendar));*/
      Calendar *new_calendar = malloc(sizeof(Calendar));
      new_calendar->name = malloc(strlen(name) + 1);
      new_calendar->events = malloc(sizeof(Event) * days);
      /*If any allocation failed, return FAILURE and end function*/
      if (!new_calendar || !new_calendar->name || !new_calendar->events) {
         return FAILURE;
      }
      for (i = 0; i < days; i++) {
         new_calendar->events[i] = NULL;
      }
      /*Initialize arguments to the corresponding fields in calendar*/
      new_calendar->total_events = 0;  
      new_calendar->days = days;
      strcpy(new_calendar->name, name);
      new_calendar->comp_func = comp_func;
      new_calendar->free_info_func = free_info_func;
      *calendar = new_calendar;
      return SUCCESS;
   }
   return FAILURE;
}

/*
 *A function that prints out every fields within calendar
 */
int print_calendar(Calendar *calendar, FILE *output_stream,
                   int print_all) {
   int days = 1, i = 0;
   if (calendar && output_stream) {
      /*Event **all_events = calendar->events;*/
      Event *current = NULL; 
      /*if print_all is true, print the name, days, 
        number of events of calendar*/
      if (print_all) {     
         fprintf(output_stream, "Calendar's Name: \"%s\"\n", calendar->name);
         fprintf(output_stream, "Days: %d\n", calendar->days);
         fprintf(output_stream, "Total Events: %d\n\n", calendar->total_events);
      }
      fprintf(output_stream, "**** Events ****\n");
      if (calendar->total_events > 0) {
      /*Loop traverses through the array of pointers*/
         for (i = 0; i < calendar->days; i++) {
            current = calendar->events[i];
            fprintf(output_stream, "Day %d\n", days++);
            /*Traverses through every Nodes(Events) and print out 
            their info*/
            while (current) { 
               fprintf(output_stream, 
                       "Event's Name: \"%s\", Start_time: %d, Duration: %d\n",
                       current->name, current->start_time, 
                       current->duration_minutes);                  
               current = current->next;
            }              
         }
      }      
      return SUCCESS;
   }
   return FAILURE;
}
 
/*
 *A function that adds event in increasing order to the calendar
 */
int add_event(Calendar *calendar, const char *name, int start_time,
              int duration_minutes, void *info, int day) {
  
   if (calendar && name && start_time >= 0 && 
       start_time <= 2400 && duration_minutes >= 1 && day >= 1 && 
       day <= calendar->days && calendar->comp_func &&
       find_event(calendar, name, NULL) == FAILURE) {
      
      Event *new_event = malloc(sizeof(Event));
      Event *prev = NULL;
      Event *current = calendar->events[day - 1];
      
      new_event->name = malloc(strlen(name) + 1);
      /*return Failure if allocation of new_event or its name fails*/
      if (!new_event || !new_event->name) {
         return FAILURE; 
      }
    
      /*Set all the fields of new_event according to the parameters*/
      new_event->start_time = start_time;
      new_event->duration_minutes = duration_minutes;
      new_event->info = info;
      strcpy(new_event->name, name);
      
      /*Traverse through the loop to add in increase order*/
      while(current && calendar->comp_func(new_event, current) > 0) {        
         prev = current;
         current = current->next;
      }
      
      new_event->next = current;
      /*If current is head*/
      if (!prev) {
         calendar->events[day - 1] = new_event; 
      }
      /*If not add it before current*/
      else {
         prev->next = new_event;
      }
      calendar->total_events++;
      return SUCCESS;
   }
   return FAILURE;
}

/*
 *A function that finds if a specific event exists in the calendar
 */
int find_event(Calendar *calendar, const char *name, Event **event) {
   int i;
   if (calendar && name) {
      /*Traverse through each day and find 
       *if the event exists in that day*/
      for (i = 1; i <= calendar->days; i++) {
         if (find_event_in_day(calendar, name, i, event) == SUCCESS) {
            return SUCCESS;
         }
      }
   }
   return FAILURE;
}

/*
 *A function that finds if a specific event exists in a specific day
 */
int find_event_in_day(Calendar *calendar, const char *name, int day,
                      Event **event) {
   if (calendar && name && day >= 1 && day <= calendar->days) {
      Event *current = calendar->events[day - 1];
      /*Traverse through the linked list*/
      while (current) {
         /*if the name is found*/
         if (!strcmp(current->name, name)) {
            /*if event is not NULL*/
            if (event) { 
               *event = current;
            }           
            return SUCCESS;
         }  
         /*Advance to the next event*/
         current = current->next;
      }     
   }
   return FAILURE;

}

/*
 *A function that removes an event with the specified name
 */
int remove_event(Calendar *calendar, const char *name) {
   int i, found = 0;
   if (calendar && name && 
       find_event(calendar, name, NULL) == SUCCESS) {
      Event *current = NULL, *prev = NULL;
      /*Traverse through each day*/
      for (i = 1; i <= calendar->days; i++) {
         prev = NULL;
         current = calendar->events[i - 1];
         /*Traverse through the linked list*/
         while (current) {
            /*End the loop if name is found*/
            if (!strcmp(current->name, name)) {
              found = 1;
              break;
            }
            prev = current;
            current = current->next;
         }
         /*end the loop if name is found*/
         if (found) {          
            break;
         }
      }
      
      if (!prev) { /*if current is the head*/
         calendar->events[i - 1] = current->next;
      }  
      else {
         prev->next = current->next;
      }

      if (calendar->free_info_func && current->info) {
         calendar->free_info_func(current->info);
      }
      free(current->name);
      free(current);
      calendar->total_events--;
      return SUCCESS;   
   }
   return FAILURE;
}

/*
 *A function that get the info of the event
 */
void *get_event_info(Calendar *calendar, const char *name) {
   Event *found_event = NULL;
   if (find_event(calendar, name, &found_event) == SUCCESS) {
      return found_event->info;
   }
   /*return NULL if event cannot be found*/
   return NULL;
}

/*
 *A function that remove every events in the calendar
 */
int clear_calendar(Calendar *calendar) {
   int i;
   if (calendar) {
      /*Traverse through each day*/
      for (i = 1; i <= calendar->days; i++) {
         /*call clear_day to delete every event
          *in each day*/
         clear_day(calendar, i);
      }     
      return SUCCESS;
   }
   return FAILURE;
}

/*
 *A function that remove every event in specific day
 */
int clear_day(Calendar *calendar, int day) {
   if (calendar && day >= 1 && day <= calendar->days) {
      Event *current = calendar->events[day - 1];
      /*Traverse through the linked list*/
      while (current) {
         /*Remove the event*/
         remove_event(calendar, current->name);   
         /*Advance to the next even*/  
         current = calendar->events[day - 1];  
      }
      /*Set the list to empty*/
      calendar->events[day - 1] = NULL;
      return SUCCESS;
   }
   return FAILURE;
}

/*
 *A function that remove every events 
 *in the calendar and destroy calendar
 */
int destroy_calendar(Calendar *calendar) {
   if (calendar) {
      /*Remove every event*/
      clear_calendar(calendar);
      /*Free everything in and calendar*/
      free(calendar->name);
      free(calendar->events);
      free(calendar);	
      return SUCCESS;
   }  
   return FAILURE;

}
