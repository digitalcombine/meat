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

/** @file */

#include <meat/memory.h>
//#include <meat/types.h>
#include <meat/numeric.h>

#include <map>
#include <set>
#include <string>
#include <stdexcept>
#include <vector>
#include <deque>
#include <iostream>
#if defined (_WIN32) || defined (_WIN64)
#include <windows.h>
#endif

#ifndef _MEAT_H
#define _MEAT_H

#if defined (_WIN32) || defined (_WIN64)
#  ifdef DLL_EXPORT
#    define DECLSPEC __declspec(dllexport)
#  else
#    define DECLSPEC __declspec(dllimport)
#  endif
#elif defined(__linux__) || defined(__FreeBSD__)
#   define DECLSPEC __attribute__((visibility("default")))
#else
#   error("Don't know how to export shared object libraries")
#endif

/** @ns */
namespace meat {

  class DECLSPEC Object;
  class DECLSPEC Class;
  class DECLSPEC Context;
  class DECLSPEC BlockContext;
  class DECLSPEC Exception;
  namespace data { // Found in datastore.h
    class Library;
    class Archive;
  }

  /** A reference counter pointer to Script Objects and classes.
   */
  typedef memory::reference<Object> Reference;

  template <class Ty>
  Ty &cast(Reference object) { return dynamic_cast<Ty &>(*(object)); }

  /** Initializes the Meat virutal machine.
   */
  void DECLSPEC initialize(int argc, const char *argv[]);

  void DECLSPEC cleanup();

  //@{
  /** Creates a new message context to an Object.
   * @param Object The Object to send the message to.
   * @param hash_id The hashed id of the method name.
   * @param context The calling context.
   * @return A new context to be executed by meat::execute().
   * @see meat::execute()
   * @see hash()
   */
  Reference DECLSPEC message(Reference object,
                             std::uint32_t hash_id,
                             Reference context);

  Reference DECLSPEC message(Reference object,
                             const std::string &method,
                             Reference context);

  Reference DECLSPEC message_super(Reference object,
                                   std::uint32_t hash_id,
                                   Reference context);
  //@}

  /** Executes a context that has been created by meat::message().
   * @param context The context created by message.
   * @return The results of the message.
   * @see meat::message()
   */
  Reference DECLSPEC execute(Reference context);

#ifdef DEBUG
  std::ostream &operator <<(std::ostream &out, Class &cls);
#endif /* DEBUG */

  /** Base Object class for all objects within the virtual machine.
   */
  class DECLSPEC Object : public memory::recyclable {
  public:
    /** Creates a new Object of the given class type.
     * @param type A Reference to the class for the new Object.
     */
    explicit Object(Reference type);

    /**
     */
    Object(Reference type, std::uint8_t properties);

    /** Destroy and clean up the Object.
     */
    virtual ~Object () throw ();

    /** Returns a reference to the class type for the Object.
     */
    Reference type() const { return _type; };

    /** Tests Object if it is of type class.
     */
    bool is_type(Reference cls) const;
    bool is_type(const std::string &class_name) const;

    virtual bool is_class() const { return false; };

    virtual bool is_object() const { return true; };

    /** Serialize the Object data to a data store.
     * @param store The store where Object is getting serialized to. This is
     *              given to the serialize method for the purpose of added
     *              the Objects properties to the store's index.
     * @param data_stream The ostream to write the Objects native data.
     */
    virtual void serialize(data::Archive &store,
                           std::ostream &data_stream) const;

    /** Populates the Object with native data from a data store. Note that the
     * properties are handled by the data store and only native data should be
     * read in here.
     */
    virtual void unserialize(data::Archive &store,
                             std::istream &data_stream);

    /** Returns the number of properties the Object contains.
     * @return Return the number of properties.
     */
    std::uint8_t properties() const { return _properties; }
    virtual Reference &property(std::uint8_t index);
    virtual const Reference &property(std::uint8_t index) const;

    friend void initialize(int argc, const char *argv[]);

  private:
    meat::Reference _type;
    std::uint8_t _properties;
    meat::Reference *_property;
  };

  /****************************************************************************
   * meat Class
   */

  /** Script Object class primative method pointer.
   * @param context The context the method is currently executing with in.
   * @return The results from the primative method.
   * @see meat::Context
   * @see meat::Class::VTable
   */
  typedef Reference (*method_ptr_t)(Reference context);

  /** Function type for creating new Objects from the class.
   */
  typedef Reference (*constructor_t)(Reference &cls, std::uint8_t properties);

#define VTM_END      (0x00)
#define VTM_BYTECODE (0x01) // Vtable entry points to bytecode
#define VTM_NATIVE   (0x02) // Vtable entry points to a native function.
#define VTM_SUPER    (0x04) // Vtable entry needs to be synced from super.

  typedef struct _vtable_entry_s {
    std::uint32_t hash_id; // All entries must be sorted by this id.
    std::uint32_t class_id;
    std::uint8_t flags;
    std::uint8_t locals;   // This includes parameters with local variables
    union {
      meat::method_ptr_t pointer;
      std::uint16_t offset;
    } method;
  } vtable_entry_t;

  typedef struct _symbol_entry_s {
    std::uint32_t hash_id;
    const char *symbol;
  } symbol_entry_t;

  typedef enum {STATIC, DYNAMIC, COPY} alloc_t;

  /**
   */
  class DECLSPEC Class : public Object {
  public:
    typedef std::map<std::uint32_t, meat::Reference>::iterator iterator;

    /** Creates a new class Object.
     */
    Class(const std::string &parent, std::uint8_t obj_props = 0);
    Class(const std::string &parent, std::uint8_t cls_props,
          std::uint8_t obj_props);
    Class(Reference parent, std::uint8_t obj_props = 0);
    Class(Reference parent, std::uint8_t cls_props, std::uint8_t obj_props);

    virtual ~Class() throw ();

    /** Get the meat super class for this class.
     * @return A Reference to the class's super class.
     */
    Reference super() const { return _super; }

    virtual std::uint8_t obj_properties() const;

    virtual bool is_class() const { return true; };
    virtual bool is_object() const { return false; };

    /** Creates a new Object of this class type. To create the actual Object
     * the constructor function, set by set_constructor(), is called to
     * create the actual Object.
     * @return A reference to the new Object.
     * @see set_constructor
     */
    Reference new_object();

    /** Gets the internal 32bit hash id for the class.
     */
    std::uint32_t hash_id() const { return _hash_id; };
    std::string name() const;
    void name(const std::string &new_name);

    /** Set the constructor function. The constructor is used by the method
     * new_object() to create the actual internal Object. If a constructor
     * function is not set the parent meat classes will be checked for a
     * constuctor function. Most of the time the default Object constructor
     * is enough unless a unique C++ class was created for the Object.
     *
     * @param constructor Pointer to the contructor function.
     * @see new_object()
     */
    void set_constructor(constructor_t constructor);
    void set_vtable(std::uint8_t entries, vtable_entry_t table[],
                    alloc_t table_alloc);
    void set_class_vtable(std::uint8_t entries, vtable_entry_t table[],
                          alloc_t table_alloc);

    void bytecode(std::uint16_t size, std::uint8_t *code, alloc_t code_alloc);
    const std::uint8_t *bytecode() const;

    const vtable_entry_t *get_vtable(uint8_t &count) const;
    const vtable_entry_t *get_class_vtable(uint8_t &count) const;

    virtual void serialize(data::Archive &store,
                           std::ostream &data_stream) const;

    virtual void write(std::ostream &lib_file) const;
    static Class *import(std::istream &lib_file);

    /** Lookup the symbol for the given hash_id. If there's no symbol for the
     * hash_id then the hash_id is returned as a hexidecimal string.
     */
    std::string lookup(std::uint32_t hash_id) const;

    //@{
    /** Register a class with the give id name.
     * @param cls The class being registered.
     * @param id The name of the class used in the Script engine.
     * @param replace By default an error is raised if an existing recorded
     *        class already exists. If replace is set to true the this is
     *        overrided. This is for internal initialization only.
     */
    static void record(Class *cls, const std::string &id,
                       bool replace = false);
    static void record(Reference &cls, const std::string &id,
                       bool replace = false);
    static void record(Reference &cls, bool replace = false);
    //@}

    static void unrecord(Reference cls, bool compiled = false);

    void relink();

    /** Resolves a class that has been registered with record.
     * @param id The text name of the class.
     * @see record()
     * @exception Exception If the class cannot be found.
     */
    static Reference &resolve(const std::string &id, bool compiled = true);

    /** Resolves a class that has been registered with record.
     * @param hash_id The integer hash id of the class.
     * @see record()
     * @see hash()
     * @exception Exception If the class cannot be found.
     */
    static Reference &resolve(std::uint32_t hash_id, bool compiled = true);

    /**
     */
    static bool have_class(const std::string &id);

    static iterator begin();
    static iterator end();

    /** Returns a reference to the class. This uses the resolve method
     * to find the reference so the class @b must be registered with record()
     * before this is called.
     * @see resolve()
     * @see record()
     */
    operator Reference &() const;

#ifdef DEBUG
    friend std::ostream &operator <<(std::ostream &out, Class &cls);
#endif /* DEBUG */
    friend void initialize(int argc, const char *argv[]);
    friend Reference message(Reference object,
                             std::uint32_t hash_id,
                             Reference context);
    friend Reference message_super(Reference object,
                                   std::uint32_t hash_id,
                                   Reference context);
    friend Reference execute(Reference context);
    friend class data::Library;

  private:
    /** The virtual method table for class Objects.
     */
    class VTable {
      public:
        VTable();
        virtual ~VTable() throw();

        void set_vtable(std::uint8_t entries, vtable_entry_t table[],
                        alloc_t table_alloc);
        void set_class_vtable(std::uint8_t entries, vtable_entry_t table[],
                              alloc_t table_alloc);
        void set(constructor_t constructor_func);

        void link(Class &super);

        /** Returns the virtual table entry if the method was found.
         */
        vtable_entry_t *find(std::uint32_t hash_id);
        const vtable_entry_t *find(std::uint32_t hash_id) const;
        vtable_entry_t *class_find(std::uint32_t hash_id);
        const vtable_entry_t *class_find(std::uint32_t hash_id) const;

        void write(std::ostream &lib_file) const;
        void read(std::istream &lib_file);

        friend class Class;
        friend Reference message(Reference object,
                                 std::uint32_t hash_id,
                                 Reference context);
        friend Reference message_super(Reference object,
                                       std::uint32_t hash_id,
                                       Reference context);
      private:
        // Object virtual table entries
        std::uint8_t no_entries;
        vtable_entry_t *entries;
        bool e_is_static;

        // Class virtual table entries.
        std::uint8_t no_centries;
        vtable_entry_t *centries;
        bool ce_is_static;

        constructor_t constructor;
    };

    const vtable_entry_t *find(std::uint32_t hash_id) const;
    const vtable_entry_t *class_find(std::uint32_t hash_id) const;

    std::string _name;
    std::uint32_t _hash_id;
    Reference _super;
    std::uint8_t _obj_properties;
    data::Library *library;

    VTable vtable;

    std::uint16_t _bytecode_size;
    std::uint8_t *_bytecode;
    bool _bytecode_static;
  };

  /****************************************************************************
   */

  /** When a message is created for a given object, a context is created for
   * the execution of the message. The context contains all the local
   * variables needs for the answering method.
   *  For bytecode methods the context also contains a reference to the
   * method's bytecode and a pointer to the current bytecode being
   * interpreted. Native methods just have a pointer to the method.
   *
   * How the local variables are setup:
   * - self - A reference to the object being messaged.
   * - class - A reference to the class object of the method answering the
   *           message.
   * - context - A weak reference to itself. Used so the answering method has
   *             access to the context.
   * - null - A reference to the null object.
   * - locals required for the method.
   */
  class DECLSPEC Context : public Object {
  public:
    /** Create a new context with no calling context.
     * @locals The number of local variables to allocate.
     */
    explicit Context(std::uint8_t locals);
    Context(Reference context, std::uint8_t locals);
    Context(Reference cls, Reference context, std::uint8_t locals);
    virtual ~Context() throw();

    virtual Reference self() const;
    virtual void self(Reference new_self);

    virtual Reference klass() const { return _locals[1]; }
    virtual Reference context() const { return _locals[2]; }
    virtual Reference super() const {
      return cast<Class>(klass()).super();
    };

    /** Get the calling context.
     */
    virtual Reference messenger() const;
    virtual void messenger(Reference context);

    virtual Reference parameter(uint8_t index) const;

    virtual void parameter(uint8_t index, Reference value);

    /** Get a local variable from the context. If the index is out of range
     * then an meat::Exception is raised.
     * @param index The index to the locals variable desired.
     * @return A reference to the local variable.
     * @see get_num_of_locals()
     */
    virtual Reference &local(uint8_t index) const;

    /** Returns the number of local variables the context contains. This is
     * set when the context is created and cannot be changed.
     * @return The number of local variables.
     */
    virtual uint8_t locals() const { return num_of_locals; };

    uint16_t ip() const { return _ip; };
    void ip(uint16_t offset) { _ip = offset; };

    void result_index(uint8_t local_parent_index);
    void result(Reference value);
    Reference result() const;

    virtual bool is_done() const { return done; };
    virtual void finish() { done = true; };
    virtual void unfinish() { done = false; };
    virtual void reset() {};

    virtual Reference operator ()(Reference context);

    friend class Class;
    friend class BlockContext;
    friend Reference message(Reference object,
                             uint32_t hash_id,
                             Reference context);
    friend Reference message_super(Reference object,
                                   uint32_t hash_id,
                                   Reference context);
    friend Reference execute(Reference context);

  protected:
    /** The context that this context was created in. If this is Null then
     * the context is the top level context of the program.
     */
    Reference _messenger;
    Reference _result;

    typedef enum {
      NOMETHOD  = 0x00,
      PRIMATIVE = 0x01,
      BYTECODE  = 0x02
    } method_t;

    method_t flags;
    meat::method_ptr_t pointer;
    uint16_t _ip;

  private:
    uint8_t num_of_locals;
    Reference *_locals;
    uint8_t _result_index;

    bool done;
  };

  /****************************************************************************
   */

  /**
   * @todo I really need to document how this guy works.
   */
  class DECLSPEC BlockContext : public Context {
  public:
    BlockContext(Reference context, uint8_t locals, uint16_t ip);
    virtual ~BlockContext() throw();

    virtual Reference self() const {
      return cast<const Context>(_origin).self();
    }

    virtual void self(Reference new_self) {
      cast<Context>(_origin).self(new_self);
    }

    virtual Reference klass() const {
      return cast<const Context>(_origin).klass();
    }
    virtual Reference context() const {
      return cast<const Context>(_origin).context();
    }
    virtual Reference super() const {
      return cast<const Class>(klass()).super();
    }

    virtual Reference parameter(uint8_t index) const {
      return _locals[index];
    };

    virtual void parameter(uint8_t index, Reference value) {
      _locals[index] = value;
    };

    /** Set a local variable with the given value. The index for the local
     * variable starts in the containing context then appending the block
     * context's variables after.
     * @param index The local index of the variable.
     * @param value The value to set the local variable to.
     * @throw Exception If the index is out of range.
     */
    virtual Reference &local(uint8_t index) const;

    /** Returns the number of locals in this context and in the containing
     * context.
     * @return uint8_t Total number of local variables.
     */
    virtual uint8_t locals() const;

    Reference origin() const { return _origin; }

    void c_break();
    void c_continue();
    void set_break_trap();
    void set_continue_trap();
    bool break_trap_set() const;
    bool continue_trap_set() const;
    bool break_called() const;
    bool continue_called() const;
    virtual void reset();

  private:
    Reference _origin;
    uint8_t bc_flags;
    uint16_t start_ip;
  };

  /****************************************************************************
   */

  class DECLSPEC Exception : public std::exception, public Object {
  public:
    Exception();
    Exception(const Exception &other);
    Exception(Reference cls, std::uint8_t properties);
    Exception(const std::string &message);
    Exception(const std::string &message, Reference context);
    Exception(Reference message, Reference context);
    virtual ~Exception() throw() {}

    virtual const char* what() const throw();
  };

  /****************************************************************************
   */

  class DECLSPEC BlockParameter : public Object {
  public:
    BlockParameter(std::uint8_t offset);
    BlockParameter(Reference cls, std::uint8_t properties);
    virtual ~BlockParameter() noexcept;

    void set_parameter(Reference block, Reference value) const;
  };

  /****************************************************************************
   */

  class DECLSPEC Value : public Object {
  public:
    Value(Reference cls, uint8_t properties);
    Value(int32_t value);
    Value(double value);
    Value(bool value);
    virtual ~Value() throw() {}

    virtual void serialize(data::Archive &store,
                           std::ostream &data_stream) const;
    virtual void unserialize(data::Archive &store, std::istream &data_stream);

    operator std::int32_t() const;
    operator double() const;

  private:
    typedef enum {
      BOOLEAN   = 0x00,
      INTEGER   = 0x01,
      FLOAT     = 0x02
    } data_type_t;

    data_type_t data_type;
    numeric::integer_32 _integer_value;
    union {
      bool b;
      //uint32_t i;
      double f;
    } data;
  };

#define INTEGER(ref) ((std::int32_t)(dynamic_cast<meat::Value &>(*(ref))))
#define FLOAT(ref) ((double)(dynamic_cast<meat::Value &>(*(ref))))

  /****************************************************************************
   */

  class DECLSPEC Text : public std::string, public Object {
  public:
    Text();
    Text(const Text &other);
    Text(Reference cls, uint8_t properties);
    Text(const std::string &value);
    virtual ~Text() throw() {}

    virtual void serialize(data::Archive &store,
                           std::ostream &data_stream) const;
    virtual void unserialize(data::Archive &store, std::istream &data_stream);
  };

  /****************************************************************************
   */

  class DECLSPEC List : private std::deque<Reference>, public Object {
  public:
    List();
    List(const List &other);
    List(Reference cls, uint8_t properties);
    virtual ~List() throw() {}

    using std::deque<Reference>::push_back;
    using std::deque<Reference>::push_front;
    using std::deque<Reference>::pop_back;
    using std::deque<Reference>::pop_front;
    using std::deque<Reference>::clear;
    using std::deque<Reference>::at;
    using std::deque<Reference>::operator[];
    using std::deque<Reference>::operator=;
    using std::deque<Reference>::insert;
    using std::deque<Reference>::erase;
    using std::deque<Reference>::back;
    using std::deque<Reference>::front;
    using std::deque<Reference>::size;
    using std::deque<Reference>::empty;
    using std::deque<Reference>::begin;
    using std::deque<Reference>::end;

    using std::deque<Reference>::iterator;
    using std::deque<Reference>::const_iterator;

    virtual void serialize(data::Archive &store,
                           std::ostream &data_stream) const;
    virtual void unserialize(data::Archive &store, std::istream &data_stream);
  };

  /****************************************************************************
   */

  class DECLSPEC obj_less {
  public:
    bool operator()(const Reference &first, const Reference &second) const;
  };

  class DECLSPEC Set : private std::set<Reference, obj_less>, public Object {
  public:
    Set();
    Set(const Set &other);
    Set(Reference cls, uint8_t properties);
    virtual ~Set() throw() {}

    using std::set<Reference, obj_less>::clear;
    using std::set<Reference, obj_less>::operator=;
    using std::set<Reference, obj_less>::insert;
    using std::set<Reference, obj_less>::erase;
    using std::set<Reference, obj_less>::size;
    using std::set<Reference, obj_less>::empty;
    using std::set<Reference, obj_less>::begin;
    using std::set<Reference, obj_less>::end;
    using std::set<Reference, obj_less>::find;

    using std::set<Reference, obj_less>::iterator;
    using std::set<Reference, obj_less>::const_iterator;

    virtual void serialize(data::Archive &store,
                           std::ostream &data_stream) const;
    virtual void unserialize(data::Archive &store, std::istream &data_stream);

    void swap(Set &other) {
      std::set<Reference, obj_less>::swap(other);
    }
  };

  /****************************************************************************
   */

  class DECLSPEC Index : private std::map<Reference, Reference, obj_less>,
                         public Object {
  public:
    Index();
    Index(Reference cls, uint8_t properties);
    virtual ~Index() throw() {}

    using std::map<Reference, Reference, obj_less>::clear;
    using std::map<Reference, Reference, obj_less>::find;
    using std::map<Reference, Reference, obj_less>::operator[];
    using std::map<Reference, Reference, obj_less>::insert;
    using std::map<Reference, Reference, obj_less>::erase;
    using std::map<Reference, Reference, obj_less>::size;
    using std::map<Reference, Reference, obj_less>::empty;
    using std::map<Reference, Reference, obj_less>::begin;
    using std::map<Reference, Reference, obj_less>::end;

    using std::map<Reference, Reference, obj_less>::iterator;
    using std::map<Reference, Reference, obj_less>::const_iterator;

    virtual void serialize(data::Archive &store,
                           std::ostream &data_stream) const;
    virtual void unserialize(data::Archive &store, std::istream &data_stream);
  };

  class DECLSPEC CompilerInterface {
  public:
    virtual void import(const std::string &library,
                        meat::Reference context) = 0;
    virtual void include(const std::string &code) = 0;
    virtual void create_class(meat::Reference super,
                              const std::string &cls_name,
                              const std::string &cls_body,
                              meat::Reference context) = 0;
    virtual void set_application_class(meat::Reference klass) = 0;
  };

  void DECLSPEC compiler(CompilerInterface *impl);
  CompilerInterface DECLSPEC *compiler();

  Reference DECLSPEC Boolean(bool value);

  bool DECLSPEC Boolean(Reference value);

  Reference DECLSPEC Null();

}

#endif /* _MEAT_H */
