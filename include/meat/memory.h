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

#ifndef _MEAT_MEMORY_H
#define _MEAT_MEMORY_H

namespace memory {

  template <typename Ty>
  class reference {
  private:

    /** Reference Counting Class Wrapper.
     *  Used by reference to implement reference counting around a
     * dynamically created object of type Ty.
     */
    class __reference {
    public:

      /** Creates a wrapper around an existing native object.
       * obj    Pointer to the native object to add reference count to.
       * irefs  The initial reference count.
       */
      __reference(Ty *obj, unsigned int irefs = 0)
        : refs(irefs), wrefs(0), nobj(obj) {
      }

      /* Destroy the native object. */
      virtual ~__reference() throw() {
        if (nobj) delete nobj;
      }

      bool is_null() { return (nobj == 0); }

      void free() {
        /* We have to set nobj to null before deleting it to prevent any kind
        * of race conditions with weak pointers.
        */
        Ty *obj = nobj;
        nobj = 0;
        delete obj;
      }

      /* Increment the weak reference counter by 1. */
      void inc_weak_ref() { wrefs++; }

      /* Decrease the weak reference counter by 1. */
      void dec_weak_ref() { if (wrefs > 0) wrefs--; }

      /* Get the current weak reference count. */
      const unsigned int __wref_cnt() const { return wrefs; }

      /* Increment the reference counter by 1. */
      void inc_ref() { refs++; }

      /* Decrease the reference counter by 1. */
      void dec_ref() { refs--; }

      /* Get the current reference count. */
      const unsigned int __ref_cnt() const { return refs; }

      /* Cast this as a reference to the object being wrapped. */
      operator Ty &() { return (Ty &)*nobj; }

      /* Cast this as a pointer to the object being wrapped. */
      operator Ty*() { return (Ty *)nobj; }

    private:
      unsigned int refs;  // The number of references.
      unsigned int wrefs; // The number of weak references.
      Ty *nobj;           // The actual object.
    };

  public:
    /** Create a null reference pointer.
     */
    reference(void) : weak_ref(false), obj(0) {
    }

    /** Create a new reference to a newly allocated object.
     */
    reference(Ty *obj) : weak_ref(false), obj(0) {
      if (obj != 0) {
        this->obj = new __reference(obj, 1);
      }
    }

    /** Default copy constructor.
     */
    reference(const reference<Ty> &obj)
			: weak_ref(obj.weak_ref), obj(obj.obj) {
      inc_reference();
    }

    /** Decreases the reference count and clean up the object if necessary.
     */
    ~reference(void) throw() {
      if (obj) {
        if (weak_ref) {
          obj->dec_weak_ref();
          if (obj->__ref_cnt() == 0 and obj->__wref_cnt() == 0)
            delete obj; obj = 0;
        } else {
          obj->dec_ref();
          if (!obj->__ref_cnt()) {
            if (obj->__wref_cnt())
              obj->free();
            else
              delete obj; obj = 0;
          }
        }
      }
    }

    /** Get the number of references to a object.
     * @return The number of references
     */
    unsigned int references(void) {
      return (obj ? obj->__ref_cnt() : 0);
    }

    /** Test if we reference an object or just a NULL pointer.
     * @return If true if the reference in pointing to null.
     */
    bool is_null(void) const {
      return ((obj == 0) or obj->is_null());
    }

    /** Creates a new weak reference.
     */
    reference weak() {
      reference result;
      result.obj = obj;
      result.weak_ref = true;
      result.inc_reference();
      return result;
    }

    /** Tests if the reference is a weak reference.
     * @return True if the reference is a weak reference otherwise false.
     */
    bool is_weak() {
      return weak_ref;
    }

    /** Decrement any current reference and create a new reference for the
     * the new unreference object. @b Note that this could be dangerous
     * if there is anothor reference already referencing @c obj. If
     * another reference is referencing @c obj then this will cause obj
     * to be destroyed twice.
     *
     * Example
     * @code
     * class MyClass;
     *
     * typedef reference<MyClass> SPMyClass;
     * class MyClass {
     * public:
     *   virtual ~MyClass(void) throw() {}
     *   SPMyClass get_reference() {
     *     return this;
     *   }
     * };
     *
     * void sample(void) {
     *   SPMyClass obj1;
     *   SPMyClass obj2 = obj1.get_reference();
     *   // Now both obj1 and obj2 will have a reference count of one and
     *   // both pointing to the same object. Now here we will end up
     *   // a segfault or worse...
     * }
     * @endcode
     * Basically you'll want to be very careful when passing around the
     * @c this pointer when using References. This is the greatest trigger
     * of this feature (@e or @e bug ;).
     *
     * @param obj A newly created object we want to reference.
     * @return A C++ reference of this.
     */
    reference<Ty> &operator=(Ty *obj) {
      dec_reference();
      if (obj) {
        this->obj = new __reference(obj, 1);
      } else
        this->obj = 0;
      return *this;
    }

    /** Decrement any current reference and reference the passed object.
     */
    reference<Ty> &operator=(const reference<Ty> &obj) {
      if (this != &obj) {
        dec_reference();
        this->obj = obj.obj;
        this->weak_ref = false;
        //this->weak_ref = obj.weak_ref;
        inc_reference();
      }
      return *this;
    }

    /** Check if the reference is referencing the same object as @c other.
     */
    bool operator ==(const reference<Ty> &other) const {
      return (obj == other.obj);
    }

    /** Check if the reference is referencing @c other.
     */
    bool operator ==(const Ty *other) const {
      return ((Ty *)(obj) == other);
    }

    bool operator !=(const reference<Ty> &obj) const {
      return (this->obj != obj.obj);
    }

    /** Dereference the reference.
     */
    Ty &operator *(void) { return (Ty &)(*obj); }

    /** Constant dereference the reference.
     */
    const Ty &operator *(void) const { return (Ty &)(*obj); }
    Ty *operator ->() { return (Ty *)(*obj); }
    const Ty *operator ->(void) const { return (Ty *)(*obj); }
    operator Ty &(void) { return (Ty &)(*obj); }
    operator const Ty &(void) const { return (Ty &)(*obj); }

  private:

    void dec_reference() throw() {
      if (obj) {
        if (weak_ref) {
          obj->dec_weak_ref();
          if (obj->__ref_cnt() == 0 and obj->__wref_cnt() == 0)
            delete obj;
        } else {
          obj->dec_ref();
          if (!obj->__ref_cnt()) {
            if (obj->__wref_cnt())
              obj->free();
            else
              delete obj;
          }
        }
      }
    }

    void inc_reference() {
      if (obj) {
        if (weak_ref)
          obj->inc_weak_ref();
        else
          obj->inc_ref();
      }
    }

    bool weak_ref;
    __reference *obj;

  };
}
#endif /* _MEAT_MEMORY_H */
