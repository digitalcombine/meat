/*                                                                  -*- c++ -*-
* Copyright (c) 2017 Ron R Wills <ron.rwsoft@gmail.com>
*
* This file is part of Meat.
*
* Meat is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Meat is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Meat.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <meat/memory.h>

typedef struct memory_s {
  std::size_t size;
  time_t collected_time;
} memory_t;

std::size_t memory::gc::limit = 5 * 1024 * 1024; // 5M
time_t memory::gc::collection_age = 60; // 1 minute
std::multimap<std::size_t, void *> memory::gc::free_memory;
std::size_t memory::gc::free_size = 0;

/*********************
 * memory::gc::alloc *
 *********************/

#include <iostream>

void *memory::gc::alloc(std::size_t size) throw(std::bad_alloc) {
  unsigned char *res = 0;
  std::multimap<std::size_t, void *>::iterator it = free_memory.find(size);

  if (it != free_memory.end()) {
    res = (unsigned char *)(it->second);
    free_memory.erase(it);
    free_size -= ((memory_t*)res)->size;
  } else {
    res = (unsigned char *)::operator new(size + sizeof(memory_t));
    ((memory_t *)(res))->size = size;
  }

  return (res + sizeof(memory_t));
}

/***********************
 * memory::gc::recycle *
 ***********************/

void memory::gc::recycle(void *ptr) throw() {
  memory_t *mptr = (memory_t *)((unsigned char *)ptr - sizeof(memory_t));
  if (mptr->size + free_size < limit) {
    free_memory.insert(std::make_pair(mptr->size, mptr));
    free_size += mptr->size;
    time(&(mptr->collected_time));
  } else {
    ::operator delete(mptr);
  }
}

/***********************
 * memory::gc::collect *
 ***********************/

void memory::gc::collect(void) {
	//std::cout << "% collecting %" << std::flush;
  memory_t *mptr = NULL;
  for (std::multimap<std::size_t, void *>::iterator it = free_memory.begin();
       it != free_memory.end(); ++it) {
    mptr = (memory_t *)it->second;
    if (mptr->collected_time - time(NULL) >= collection_age) {
      ::operator delete(mptr);
    }
  }
}

/***************************
 * memory::gc::collect_all *
 ***************************/

void memory::gc::collect_all(void) {
	//std::cout << "% collecting all %" << std::flush;
  for (std::multimap<std::size_t, void *>::iterator it = free_memory.begin();
       it != free_memory.end(); ++it) {
    ::operator delete(it->second);
  }
}

/************************************
 * memory::recyclable::operator new *
 ************************************/

void *
memory::recyclable::operator new(std::size_t size) throw(std::bad_alloc) {
  return gc::alloc(size);
}

/**************************************
 * memory::recyclable::operator new[] *
 **************************************/

void *
memory::recyclable::operator new[](std::size_t size) throw(std::bad_alloc) {
  return gc::alloc(size);
}

/***************************************
 * memory::recyclable::operator delete *
 ***************************************/

void memory::recyclable::operator delete(void *ptr) throw() {
  gc::recycle(ptr);
}

/*****************************************
 * memory::recyclable::operator delete[] *
 *****************************************/

void memory::recyclable::operator delete[](void *ptr) throw() {
  gc::recycle(ptr);
}
