#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include <common.h>
#include "student_code.h"

int statusno, initialized;

node_t *_free_list;
size_t num_free_chunks = 0;
void *arena_start, *arena_end;

void print_header(node_t *header){
  //Note: These printf statements may produce a segmentation fault if the buff
  //pointer is incorrect, e.g., if buff points to the start of the arena.
  printf("Header->size: %lu\n", header->size);
  printf("Header->fwd: %p\n", header->fwd);
  printf("Header->bwd: %p\n", header->bwd);
  printf("Header->is_free: %d\n", header->is_free);
}

void head_check(node_t* free_list_head) {
    if (free_list_head->is_free) {
        return;
    }

    node_t* current = free_list_head;
    while (!free_list_head->is_free) {
        current = current->fwd;
    }

    if (current != NULL) {
        _free_list = current;
    }
}

int init(size_t size) {
  if(size > (size_t) MAX_ARENA_SIZE) {
    return ERR_BAD_ARGUMENTS;
  }


  // Find pagesize and increase allocation to match some multiple a page size
  // Question: Why is it good to match our allocation to the size of the page?
  int pagesize = getpagesize();

  if (pagesize <= 0) {
      return ERR_CALL_FAILED;
  }

  //Align to page size
  if( size % pagesize != 0 ) {
    // Calculate how much we need to increase to match the size of a page
    size -= size % pagesize;
    size += pagesize;
  }

  // Open up /dev/zero to zero-init our memory.
  int fd=open("/dev/zero",O_RDWR);
  if (fd == -1) {
    return ERR_SYSCALL_FAILED;
  }
  // Map memory from /dev/zero using mmap()
  arena_start = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (arena_start == MAP_FAILED) {
    return ERR_SYSCALL_FAILED;
  }

  arena_end = arena_start + size;
  initialized = 1;

  _free_list = arena_start;
  _free_list->size = size - sizeof(node_t);

  // Set up the initial free element
  _free_list->fwd = NULL;
  _free_list->bwd = NULL;
  _free_list -> is_free = true;

  return (int) size;
}

int destroy() {

  if (initialized == 0) {
    return ERR_UNINITIALIZED;
  }

  // Remove arena with munmap()
  if(munmap(arena_start, arena_end - arena_start) == -1) {
    return ERR_SYSCALL_FAILED;
  }

  // Question: Are there memory leaks?

  // Clean up variables
  arena_start = NULL;
  arena_end = NULL;
  _free_list = NULL;
  initialized = 0;

  return 0;
}


node_t * find_first_free_chunk(size_t size, node_t* free_list) {
    if (free_list == NULL) {
        return free_list;
    }

    node_t* result = NULL;
    node_t* current = free_list;

    do {
        if (current->size >= size && current->is_free) {
            result = current;
        }
        else {
            if (current->fwd != NULL) {
                current = current->fwd;
            }
            else {
                return NULL;
            }
        }
    } while (result == NULL);

    return result;
}

node_t* split_node(node_t* node_to_allocate, size_t size) {

    node_t* next = node_to_allocate->fwd;

    if (node_to_allocate->size == size || node_to_allocate->size - size < sizeof(node_t)) {
        //Chunk is exactly right size or big enough to allocate but not big enough to split
        node_to_allocate->is_free = false;
        return node_to_allocate;
    }
    else {
        // If the requested memory does not take up the entire free chunk, we need to
        // to split that chunk and add a new node to the free list.

        //This line cost me at least 4 hours of my life
        node_t* newnode = (void*) node_to_allocate + size + sizeof(node_t);

        newnode->size = node_to_allocate->size - size - sizeof(node_t);
        newnode->is_free = true;
        newnode->fwd = next;

        add_to_free_list(newnode, _free_list);

        node_to_allocate->size = size;
        node_to_allocate->is_free = false;
        node_to_allocate->fwd = NULL;
        node_to_allocate->bwd = NULL;

        if (node_to_allocate == _free_list) {
            newnode->bwd = NULL;
            _free_list = newnode;
        }

        return newnode;
    }

}

node_t* get_header(void* ptr) {
    if (ptr != NULL) {
        return ptr - sizeof(node_t);
    }
    else {
        return NULL;
    }
}

void coalesce_nodes(node_t* front, node_t* back) {
  if (front > back) {
    // Check to make sure they're in the right order
    statusno = ERR_BAD_ARGUMENTS;
    return;
  }
  if (front == back) {
    // Check to make sure they aren't the same node
    statusno = ERR_BAD_ARGUMENTS;
    return;
  }
  if (front == NULL || back == NULL) {
    // Then one of them is already the end of the list
    statusno = ERR_BAD_ARGUMENTS;
    return;
  }
  if ( ! (front->is_free && back->is_free)) {
    // Then one of them isn't free
    statusno = ERR_CALL_FAILED;
    return;
  }

  node_t* next = back->fwd;

  front->fwd = next;
  if (front->fwd != NULL) {
      next->bwd = front;
  }
  

  front->size = (front->size + back->size + sizeof(node_t));

  back->size = 0;
  back->is_free = false;
  back->fwd = NULL;
  back->bwd = NULL;
}


void* mem_alloc(size_t size){
  // Check to make sure we are initialized, and if not set statusno and return NULL;
  if(initialized == 0) {
      statusno = ERR_UNINITIALIZED;
    return NULL;
  }

  // Find a free chunk of memory
  node_t* node = find_first_free_chunk(size, _free_list);

  // If finding a node returned NULL then we're out of memory
  if (node == NULL) {
      statusno = ERR_OUT_OF_MEMORY;
    return NULL;
  }

  // Split node to be the appropriate size, since there's no guarantee a free node is the right size already
  split_node(node, size);

  //todo: fix this return to point at the memory we just allocated
  return (void*) node + sizeof(node_t);
}

void mem_free(void *ptr){

  if (ptr == NULL){
    statusno = ERR_CALL_FAILED;
    return;
  }

  if (ptr < arena_start || ptr > arena_end){
    // Then the pointer is outside of the arena
    statusno = ERR_CALL_FAILED;
    return;
  }

  // Step forward from the pointer to look at the node header
  node_t* newchunk = get_header(ptr);
  
  // Set the node to be free
  newchunk->is_free = true;
  
  // Insert chunk into free list
  add_to_free_list(newchunk, _free_list);

  // Coalesce together the chunks
  if ((void*) newchunk->fwd == (void*) newchunk + newchunk->size + sizeof(node_t)) {
      coalesce_nodes(newchunk, newchunk->fwd);
  }

  if ((void*) newchunk->bwd != NULL && (void*) newchunk->bwd == (void*) newchunk - newchunk->bwd->size - sizeof(node_t)) {
      coalesce_nodes(newchunk->bwd, newchunk);
  }
 
}

void add_to_free_list(node_t* newly_freed_node, node_t* free_list_head) {
    if (free_list_head == NULL) {
        _free_list = newly_freed_node;
        return;
    }

    if (newly_freed_node < free_list_head) {
        newly_freed_node->fwd = free_list_head;
        newly_freed_node->bwd = NULL;
        free_list_head->bwd = newly_freed_node;
        _free_list = newly_freed_node;
    }
    else {
        node_t* current = free_list_head;
        while (current->fwd != NULL) {
            if (newly_freed_node == current->fwd) {
                return;
            }
            if (newly_freed_node > current->fwd) {
                current = current->fwd;
            }
            else {
                newly_freed_node->fwd = current->fwd;
                current->fwd->bwd = newly_freed_node;
                current->fwd = newly_freed_node;
                newly_freed_node->bwd = current;
                return;
            }
            
        }
        current->fwd = newly_freed_node;
        newly_freed_node->bwd = current;
        newly_freed_node->fwd = NULL;
    }
}