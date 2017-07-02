
#include <meat.h>
#include <meat/datastore.h>
#include <meat/compiler.h>

#define null (meat::Null())

/******************************************************************************
 * Grinder.Library Class
 */

static meat::Reference Grinder_Library_constructor(meat::Reference &klass,
																									 meat::uint8_t properties) {
  return new meat::grinder::Library(klass, properties);
}

#define name (self->property(0))
#define classes (self->property(1))
#define documentation (self->property(2))

// method addClass:
static meat::Reference Grinder_Library_om_addClass_(meat::Reference context) {
	meat::Reference self = CONTEXT(context).get_self();
	meat::Reference klass = CONTEXT(context).get_class();
	meat::Reference theClass = CONTEXT(context).get_param(0);

	dynamic_cast<meat::grinder::Library &>(*self).add_class(theClass);
	return null;
}

// method compile
static meat::Reference Grinder_Library_om_compile(meat::Reference context) {
	meat::Reference self = CONTEXT(context).get_self();
	meat::Reference klass = CONTEXT(context).get_class();

	dynamic_cast<meat::grinder::Library &>(*self).write();
	return null;
}

// method import:
static meat::Reference Grinder_Library_om_import_(meat::Reference context) {
	meat::Reference self = CONTEXT(context).get_self();
	meat::Reference klass = CONTEXT(context).get_class();
	meat::Reference libraryName = CONTEXT(context).get_param(0);

	dynamic_cast<meat::grinder::Library &>
		(*self).add_import(libraryName->to_string());
	return null;
}

// method setApplicationClass:
static meat::Reference
Grinder_Library_om_setApplicationClass_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference className = CONTEXT(context).get_param(0);

	dynamic_cast<meat::grinder::Library &>
		(*self).set_application(className->to_string());
	return null;
}

static meat::vtable_entry_t Grinder_LibraryMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER   , 0,  {.offset = 0}},
	{0x000007a0, 0x00000000, VTM_SUPER   , 0,  {.offset = 0}},
	{0x00019850, 0x00000000, VTM_SUPER   , 0,  {.offset = 0}},
	{0x00368f3a, 0x00000000, VTM_SUPER   , 0,  {.offset = 0}},
	{0x00379f78, 0x00000000, VTM_SUPER   , 0,  {.offset = 0}},
	{0x0422afcc, 0x335f47b6, VTM_BYTECODE, 7,  {.offset = 239}},
	{0x14274dc2, 0x335f47b6, VTM_BYTECODE, 13, {.offset = 1}},
	{0x34003578, 0x335f47b6, VTM_BYTECODE, 6,  {.offset = 221}},
	{0x38a75a5d, 0x335f47b6, VTM_NATIVE  , 0,  Grinder_Library_om_compile},
	{0x39a68c12, 0x00000000, VTM_SUPER   , 0,  {.offset = 0}},
	{0x39a6a1d2, 0x00000000, VTM_SUPER   , 0,  {.offset = 0}},
	{0x49db9c88, 0x335f47b6, VTM_BYTECODE, 8,  {.offset = 146}},
	{0x6447c96a, 0x335f47b6, VTM_NATIVE  , 1,  Grinder_Library_om_addClass_},
	{0x645271d8, 0x335f47b6, VTM_NATIVE  , 1,  Grinder_Library_om_setApplicationClass_},
	{0x68f39719, 0x335f47b6, VTM_BYTECODE, 7,  {.offset = 260}},
	{0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0,  {.offset = 0}},
	{0x72cd0161, 0x335f47b6, VTM_NATIVE  , 1,  Grinder_Library_om_import_},
	{0x7a8e569a, 0x00000000, VTM_SUPER   , 0,  {.offset = 0}},
	{0x7b82e32c, 0x335f47b6, VTM_BYTECODE, 5,  {.offset = 209}},
	{0x7b840562, 0x00000000, VTM_SUPER   , 0,  {.offset = 0}}
};

#undef name
#undef classes
#undef documentation

// class method new:
static meat::Reference Grinder_Library_cm_new_(meat::Reference context) {
	meat::Reference self = CONTEXT(context).get_self();
	meat::Reference klass = CONTEXT(context).get_class();
	meat::Reference className = CONTEXT(context).get_param(0);

	meat::Reference new_object = CLASS(self).new_object();
	new_object->property(0) = className;
	(dynamic_cast<meat::grinder::Library &>(*new_object)).register_as(className->to_string());

	return new_object;

}

static meat::vtable_entry_t Grinder_LibraryCMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x000007a0, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x00019850, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x00338a9a, 0x335f47b6, VTM_NATIVE  , 1, Grinder_Library_cm_new_},
	{0x00368f3a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x2c296348, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x39a68c12, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}}
};

static meat::uint8_t Grinder_LibraryBytecode[] = {
  0x0b, 0x11, 0x05, 0x01, 0x14, 0x06, 0x00, 0x00, 0x00, 0x0d, 0x0a, 0x07, 0x04,
  0x00, 0x28, 0x02, 0x0d, 0x0e, 0x7b, 0x82, 0xe3, 0x2c, 0x00, 0x02, 0x04, 0x0f,
  0x00, 0x00, 0x07, 0xa0, 0x01, 0x0e, 0x0a, 0x10, 0x00, 0x00, 0x09, 0x01, 0x02,
  0x41, 0x79, 0x69, 0x3a, 0x01, 0x0d, 0x0b, 0x01, 0x0f, 0x7b, 0x80, 0xe9, 0x8e,
  0x01, 0x10, 0x0b, 0x01, 0x05, 0x01, 0x8b, 0x0a, 0x25, 0x02, 0x06, 0x07, 0x13,
  0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x43, 0x6c, 0x61, 0x73, 0x73, 0x20,
  0x00, 0x11, 0x07, 0x00, 0x02, 0x06, 0x08, 0x00, 0x00, 0x00, 0x2b, 0x01, 0x07,
  0x16, 0x09, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0x20,
  0x69, 0x6e, 0x20, 0x6c, 0x69, 0x62, 0x72, 0x61, 0x72, 0x79, 0x20, 0x00, 0x02,
  0x08, 0x0a, 0x00, 0x00, 0x00, 0x2b, 0x01, 0x09, 0x11, 0x0b, 0x00, 0x02, 0x0a,
  0x0c, 0x00, 0x00, 0x00, 0x2b, 0x01, 0x0b, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15,
  0x01, 0x0c, 0x0b, 0x13, 0x05, 0x00, 0x24, 0x24, 0xbe, 0x02, 0x05, 0x04, 0x00,
  0x01, 0xa9, 0xa0, 0x00, 0x11, 0x05, 0x01, 0x14, 0x06, 0x00, 0x00, 0x00, 0x08,
  0x0a, 0x07, 0x02, 0x00, 0x11, 0x02, 0x08, 0x09, 0x7b, 0x82, 0xe3, 0x2c, 0x00,
  0x01, 0x04, 0x50, 0xb5, 0x14, 0x89, 0x01, 0x09, 0x0b, 0x01, 0x05, 0x01, 0x8b,
  0x0a, 0x25, 0x02, 0x06, 0x07, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x04,
  0x0b, 0x11, 0x04, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b,
  0x13, 0x04, 0x00, 0x24, 0x24, 0xbe, 0x02, 0x04, 0x05, 0x00, 0x01, 0xa9, 0xa0,
  0x00, 0x20, 0x01, 0x05, 0x0b, 0x02, 0x00, 0x05, 0x14, 0x27, 0x4d, 0xc2, 0x01,
  0x04, 0x11, 0x06, 0x01, 0x01, 0x06, 0x41, 0x14, 0x18, 0x66, 0x01, 0x05, 0x0b,
  0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x43, 0x75, 0x72, 0x72, 0x65,
  0x6e, 0x74, 0x6c, 0x79, 0x20, 0x75, 0x6e, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x74,
  0x6f, 0x20, 0x72, 0x65, 0x6e, 0x61, 0x6d, 0x65, 0x20, 0x6c, 0x69, 0x62, 0x72,
  0x61, 0x72, 0x69, 0x65, 0x73, 0x2e, 0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15,
  0x01, 0x06, 0x02, 0x04, 0x05, 0x2c, 0x02, 0x5c, 0x80, 0x00, 0x20, 0x00, 0x05,
  0x0b
};

/******************************************************************************
 * Grinder.Class Class
 */

static meat::Reference Grinder_Class_constructor(meat::Reference &klass,
																								 meat::uint8_t properties) {
  return new meat::grinder::Class(klass, properties);
}

#define className (self->property(0))
#define superClass (self->property(1))

// class method subClassFrom:as:
static meat::Reference
Grinder_Class_cm_subClassFrom_as_(meat::Reference context) {
	meat::Reference self = CONTEXT(context).get_self();
	meat::Reference klass = CONTEXT(context).get_class();
	meat::Reference aClass = CONTEXT(context).get_param(0);
	meat::Reference name = CONTEXT(context).get_param(1);

	meat::Reference new_object = CLASS(self).new_object();
	new_object->property(0) = name;
	new_object->property(1) = aClass;
	return new_object;
}

static meat::vtable_entry_t Grinder_ClassCMethods[] = {
	{0x74b24d15, 0x335f47b6, VTM_NATIVE  , 2, Grinder_Class_cm_subClassFrom_as_},
};

#undef className
#undef superClass

/******************************************************************************
 */

extern "C" {
	DECLSPEC void init_Grinder(meat::data::Library &library);
}

void init_Grinder(meat::data::Library &library) {
	meat::Class *cls;

	/* Create the Comiler.Library class. */
	cls = new meat::Class("Object", 3);
	cls->set_constructor(Grinder_Library_constructor);
	cls->set_vtable(20, Grinder_LibraryMethods, meat::STATIC);
	cls->set_class_vtable(12, Grinder_LibraryCMethods, meat::STATIC);
	cls->set_bytecode(326, Grinder_LibraryBytecode, meat::STATIC);
	library.add(cls, "Grinder.Library");

	/* Create the Compiler.Class class */
	cls = new meat::Class("Object", 7);
	cls->set_constructor(Grinder_Class_constructor);
	cls->set_class_vtable(1, Grinder_ClassCMethods, meat::STATIC);
	library.add(cls, "Grinder.Class");

	/* Create the Compiler.Method class. */
	cls = new meat::Class("Object", 3);
	library.add(cls, "Grinder.Method");
}
