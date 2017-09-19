#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flight_map.h"

// A safe version of malloc, that will exit the program in case your allocation
// fails.
void *checked_malloc(size_t size) {
     void *ptr = malloc(size);
     if (ptr == NULL) {
          fprintf(stderr, "memory allocation failed\n");
          exit(1);
     }
     return ptr;
}

struct map_t {
     struct city** cities; //array to all of cities on map           
     int num_tcities; //number of total cities on map
     int size_path;
};

struct city {
     const char* name;
     const char** next_cities; //Cities that are linked to this city.
     int num_Lcities; //number of cities linked to this one. (also, size of array of next_cities)
     int visit; //Has this city been visited before.
};


map_t* map_create() {
     struct map_t* main_map = (struct map_t*) checked_malloc(sizeof(struct map_t));
     main_map->cities = NULL;
     main_map->num_tcities = 0;
     main_map->size_path = 0;
     return main_map;
};

void map_free(map_t* map) {
     int temp = map->num_tcities;
     for(int i = temp-1; i >=0; i--){
          remove_city(map, map->cities[i]->name);
     }
     free(map->cities);
     free(map);
}

int is_link(struct city* c1, struct city* c2){
     for (int i = 0; i < c1->num_Lcities; i++){
          if (c1->next_cities[i]==c2->name){
               return 1;
          }
     }
     for (int i = 0; i < c2->num_Lcities; i++){
          if (c2->next_cities[i]==c1->name){
               return 1;
          }
     }
     return 0;
}


struct city* find_city(map_t* map, const char* cname){
     if(map->num_tcities == 0){
          return NULL;
     } else {
          for (int i = 0; i < map->num_tcities; i++){
               if(map->cities[i]->name==cname){
                    return map->cities[i];
               }
          }
     }
     return NULL;
}

int add_city(map_t* map, const char* name) {
     if (find_city(map,name)==NULL){
          if (!map->num_tcities)  {
               map->cities = (struct city**) calloc(1, sizeof(struct city*));
          } else{
               map->cities = (struct city**) realloc(map->cities, sizeof(struct city*) * (map->num_tcities + 1));
          }
          
          map->cities[map->num_tcities] = (struct city*) checked_malloc(sizeof(struct city));
          /*map->cities[map->num_tcities]->name = name;
          map->cities[map->num_tcities]->visit = 0;
          map->cities[map->num_tcities]->next_cities=NULL; */
          *map->cities[map->num_tcities] = (struct city) {.name = name, .next_cities = NULL, .num_Lcities = 0, .visit = 0};
          map->num_tcities+=1;
          return 1;
     } else {
          return 0;
     }
}

int num_cities(map_t* map) {
     return (map->num_tcities);
}



int remove_city(map_t* map, const char* name) {
     if(find_city(map,name)==NULL){
          return 0;
     } else {
          struct city* current_city = find_city(map,name);
          struct city* last_city = map->cities[map->num_tcities-1];
          int temp = current_city->num_Lcities;
          for(int i=temp-1; i >= 0; i--){
               unlink_cities(map, name, current_city->next_cities[i]);
          }
          free(current_city->next_cities);
          free(current_city);
          current_city = last_city;
          map->num_tcities-=1;
          map->cities = (struct city**) realloc(map->cities, map->num_tcities*sizeof(struct city*));
          return 1;
     }
}

int check_cities(map_t* map, const char* city1_name, const char* city2_name){
     struct city* c1= find_city(map,city1_name);
     struct city* c2 = find_city(map,city2_name);
     if(!c1 || !c2 || c1==c2) {
          return 0;
     }
     if (is_link(c1,c2)){
          return 1;
     } else{
          return -1;
     }
}

int link_cities(map_t* map, const char* city1_name, const char* city2_name) {
     if(check_cities(map,city1_name,city2_name)>=0){  
          return 0;
     }else{
          struct city* first_city = find_city(map, city1_name);
          struct city* scnd_city  = find_city(map, city2_name);
          first_city->num_Lcities += 1;
          scnd_city->num_Lcities  += 1;
          first_city->next_cities = (const char **) realloc(first_city->next_cities,first_city->num_Lcities*sizeof(const char*));
          scnd_city->next_cities = (const char **) realloc(scnd_city->next_cities,scnd_city->num_Lcities*sizeof(const char*));
          first_city->next_cities[first_city->num_Lcities-1] = city2_name;
          scnd_city->next_cities[scnd_city->num_Lcities-1] = city2_name;
          
          return 1;
     }
}

int unlink_cities(map_t* map, const char* city1_name, const char* city2_name) {
     if(check_cities(map,city1_name,city2_name)!=1){  
          return 0;
     }else{
          const char** first_city = find_city(map, city1_name)->next_cities;
          const char** scnd_city  = find_city(map, city2_name)->next_cities;
          int size_city1 = find_city(map, city1_name)->num_Lcities;
          int size_city2 = find_city(map, city2_name)->num_Lcities;
          for (int i = 0; i < size_city1;i++){
               if(strcmp(first_city[i],city2_name)==0){
                    first_city[i] = first_city[size_city1-1];
                    find_city(map, city1_name)->num_Lcities -=1;
                    first_city = (const char**)realloc(first_city,(size_city1) * sizeof(const char*));
                    
               }
          }
          for (int i = 0; i < size_city2;i++){
               if(strcmp(scnd_city[i],city1_name)==0){
                    scnd_city[i] = scnd_city[size_city2-1];
                    find_city(map, city2_name)->num_Lcities -=1;
                    scnd_city = (const char**)realloc(scnd_city,(size_city2) * sizeof(const char*));
               }
          }
          return 1;
     }
}

const char** linked_cities(map_t* map, const char* city_name) {
     const char** result = (const char**)checked_malloc(sizeof(const char*) * (find_city(map,city_name)->num_Lcities +1));
     for(int i = 0; i < find_city(map,city_name)->num_Lcities;  i++) {
          result[i] = find_city(map,city_name)->next_cities[i];
     }
     result[(find_city(map,city_name)->num_Lcities)] = NULL;
     return result;
}

const char** find_path(map_t* map, const char* src_name, const char* dst_name) {
     struct city* city = find_city(map, src_name);
     const char** result = NULL;
     city->visit += 1;
     map->size_path += 1;
     if (strcmp(src_name,dst_name)==0){
          result = (const char**) checked_malloc((map->size_path+1) * sizeof(const char*));
          for (int i = 0; i < map->size_path + 1; i++) {
               result[i] = NULL;
          }
          result[map->size_path-1]= dst_name;
          return result;
     } else {
          for (int i =0; i < city->num_Lcities; i++){
               if(city->visit > find_city(map,city->next_cities[i])->visit){
                    result = find_path(map,find_city(map,city->next_cities[i])->name, dst_name);
                    if(result != NULL){
                         for(int z = map->size_path-2; z>=0; z--){
                              if (!result[z]){
                                   result[z]=src_name;

                                   return result;
                              }
                         }
                    }
               }
          }
     }
     map->size_path-=1;
     return NULL;

}
/*
void map_export(map_t* map, FILE* f) {
     // YOUR CODE HERE
}

map_t* map_import(FILE* f) {
     // YOUR CODE HERE
     return NULL;
}
*/