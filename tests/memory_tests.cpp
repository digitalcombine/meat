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
#include <iostream>

class TestObject {
public:
	static unsigned int count;

	TestObject() {
		++count;
	}

	virtual ~TestObject() throw() {
		--count;
	}
};

unsigned int TestObject::count = 0;

typedef memory::reference<TestObject> ref;

bool ref_examine(int &result, ref &object, bool is_null, bool weak,
								 unsigned int refs, unsigned int wrefs, unsigned int objs) {
	if (object.is_null() != is_null) {
		std::cout << "failed\n" << std::boolalpha
							<< "  is_null() -> " << object.is_null() << " != "
							<< is_null << std::endl;
		result = 1;
		return false;
	} else if (object.is_weak() != weak) {
		std::cout << "failed\n" << std::boolalpha
							<< "  is_weak() -> " << object.is_weak() << " != "
							<< weak << std::endl;
		result = 1;
		return false;
	} else if (object.references() != refs) {
		std::cout << "failed\n" << std::boolalpha
							<< "  references() -> " << object.references() << " != "
							<< refs << std::endl;
		result = 1;
		return false;
	} else if (object.weak_references() != wrefs) {
		std::cout << "failed\n" << std::boolalpha
							<< "  weak_references() -> " << object.weak_references()
							<< " != " << wrefs << std::endl;
		result = 1;
		return false;
	} else if (TestObject::count != objs) {
		std::cout << "failed\n"
							<< "  " << (unsigned int)TestObject::count << " objects != "
							<< objs << std::endl;
		result = 1;
		return false;
	}

	return true;
}

void ref_parameter(int &result, ref object) {
	if (object.is_null()) {
		std::cout << "failed\n  Parameter reference was null" << std::endl;
		result = 1;
	} else if (object.references() != 3) {
		std::cout << "failed\n  Parameter reference had "
							<< (unsigned int)object.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 1) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	}
}

ref ref_result(int &result) {
	TestObject *obj = new TestObject;
	ref ptr(obj);
	return ptr;
}

void basic_ref_counting(int &result) {
	std::cout << "=============================================================="
						<< "=================\n"
						<< "TEST SUITE: Basic Reference Counting" << std::endl;
	ref ptr1 = new TestObject;
	ref ptr2;

	std::cout << "TEST: Initial Null Reference... " << std::flush;

	if (ref_examine(result, ptr2, true, false, 0, 0, 1)) {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Initial Reference Count... " << std::flush;
	if (ref_examine(result, ptr1, false, false, 1, 0, 1)) {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Second Reference... " << std::flush;
	ptr2 = ptr1;
	if (ptr2.is_null() or ptr1.is_null()) {
		std::cout << "failed\n  A reference was null" << std::endl;
		result = 1;
	} else if (ptr2.references() != 2) {
		std::cout << "failed\n  Second reference had "
							<< (unsigned int)ptr2.references() << " references."
							<< std::endl;
		result = 1;
	} else if (ptr1.references() != 2) {
		std::cout << "failed\n  Initial reference had "
							<< (unsigned int)ptr1.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 1) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Passing Reference as a Parameter... " << std::flush;
	ref_parameter(result, ptr1);
	if (ptr2.is_null() or ptr1.is_null()) {
		std::cout << "failed\n  A reference was null" << std::endl;
		result = 1;
	} else if (ptr2.references() != 2) {
		std::cout << "failed\n  Second reference had "
							<< (unsigned int)ptr2.references() << " references."
							<< std::endl;
		result = 1;
	} else if (ptr1.references() != 2) {
		std::cout << "failed\n  Initial reference had "
							<< (unsigned int)ptr1.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 1) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Passing Other Reference as a Parameter... "
						<< std::flush;
	ref_parameter(result, ptr2);
	if (ptr2.is_null() or ptr1.is_null()) {
		std::cout << "failed\n  A reference was null" << std::endl;
		result = 1;
	} else if (ptr2.references() != 2) {
		std::cout << "failed\n  Second reference had "
							<< (unsigned int)ptr2.references() << " references."
							<< std::endl;
		result = 1;
	} else if (ptr1.references() != 2) {
		std::cout << "failed\n  Initial reference had "
							<< (unsigned int)ptr1.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 1) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Releasing Reference... " << std::flush;
	ptr1 = NULL;
	if (not ptr1.is_null()) {
		std::cout << "failed\n  Initial reference wasn't null" << std::endl;
		result = 1;
	} else if (ptr2.is_null()) {
		std::cout << "failed\n  Second reference was null" << std::endl;
		result = 1;
	} else if (ptr2.references() != 1) {
		std::cout << "failed\n  Second reference had "
							<< (unsigned int)ptr2.references() << " references."
							<< std::endl;
		result = 1;
	} else if (ptr1.references() != 0) {
		std::cout << "failed\n  Initial reference had "
							<< (unsigned int)ptr1.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 1) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Releasing Last Reference... " << std::flush;
	ptr2 = NULL;
	if (not ptr1.is_null() or not ptr2.is_null()) {
		std::cout << "failed\n  A reference wasn't null" << std::endl;
		result = 1;
	} else if (ptr2.references() != 0) {
		std::cout << "failed\n  Second reference had "
							<< (unsigned int)ptr2.references() << " references."
							<< std::endl;
		result = 1;
	} else if (ptr1.references() != 0) {
		std::cout << "failed\n  Initial reference had "
							<< (unsigned int)ptr1.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 0) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}
}

void ref_replace(int &result) {
	std::cout << "=============================================================="
						<< "=================\n"
						<< "TEST SUITE: Reference Replacing" << std::endl;
	ref ptr1 = new TestObject;
	ref ptr2;

	std::cout << "TEST: Initial Null Reference... " << std::flush;
	if (not ptr2.is_null()) {
		std::cout << "failed\n  Initial pointer wasn't null" << std::endl;
		result = 1;
	} else if (ptr2.references() != 0) {
		std::cout << "failed\n  Null reference had "
							<< (unsigned int)ptr2.references() << " references."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Initial Reference Count... " << std::flush;
	if (ptr1.is_null()) {
		std::cout << "failed\n  Initial reference was null" << std::endl;
		result = 1;
	} else if (ptr1.references() != 1) {
		std::cout << "failed\n  Initial reference had "
							<< (unsigned int)ptr1.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 1) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Second Reference... " << std::flush;
	ptr2 = ptr1;
	if (ptr2.is_null() or ptr1.is_null()) {
		std::cout << "failed\n  A reference was null" << std::endl;
		result = 1;
	} else if (ptr2.references() != 2) {
		std::cout << "failed\n  Second reference had "
							<< (unsigned int)ptr2.references() << " references."
							<< std::endl;
		result = 1;
	} else if (ptr1.references() != 2) {
		std::cout << "failed\n  Initial reference had "
							<< (unsigned int)ptr1.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 1) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Replacing Reference... " << std::flush;
	ptr1 = new TestObject;
	if (ptr1.is_null()) {
		std::cout << "failed\n  Initial reference was null" << std::endl;
		result = 1;
	} else if (ptr2.is_null()) {
		std::cout << "failed\n  Second reference was null" << std::endl;
		result = 1;
	} else if (ptr2.references() != 1) {
		std::cout << "failed\n  Second reference had "
							<< (unsigned int)ptr2.references() << " references."
							<< std::endl;
		result = 1;
	} else if (ptr1.references() != 1) {
		std::cout << "failed\n  Initial reference had "
							<< (unsigned int)ptr1.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 2) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Receiving Reference as Result... "
						<< std::flush;
	ptr1 = ref_result(result);
	if (ptr1.is_null()) {
		std::cout << "failed\n  Initial reference was null" << std::endl;
		result = 1;
	} else if (ptr2.is_null()) {
		std::cout << "failed\n  Second reference was null" << std::endl;
		result = 1;
	} else if (ptr2.references() != 1) {
		std::cout << "failed\n  Second reference had "
							<< (unsigned int)ptr2.references() << " references."
							<< std::endl;
		result = 1;
	} else if (ptr1.references() != 1) {
		std::cout << "failed\n  Initial reference had "
							<< (unsigned int)ptr1.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 2) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Ignored Reference Result... "
						<< std::flush;
	ref_result(result);
	if (ptr1.is_null()) {
		std::cout << "failed\n  Initial reference was null" << std::endl;
		result = 1;
	} else if (ptr2.is_null()) {
		std::cout << "failed\n  Second reference was null" << std::endl;
		result = 1;
	} else if (ptr2.references() != 1) {
		std::cout << "failed\n  Second reference had "
							<< (unsigned int)ptr2.references() << " references."
							<< std::endl;
		result = 1;
	} else if (ptr1.references() != 1) {
		std::cout << "failed\n  Initial reference had "
							<< (unsigned int)ptr1.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 2) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Releasing References... " << std::flush;
	ptr1 = NULL;
	ptr2 = NULL;
	if (not ptr1.is_null() or not ptr2.is_null()) {
		std::cout << "failed\n  A reference wasn't null" << std::endl;
		result = 1;
	} else if (ptr2.references() != 0) {
		std::cout << "failed\n  Second reference had "
							<< (unsigned int)ptr2.references() << " references."
							<< std::endl;
		result = 1;
	} else if (ptr1.references() != 0) {
		std::cout << "failed\n  Initial reference had "
							<< (unsigned int)ptr1.references() << " references."
							<< std::endl;
		result = 1;
	} else if (TestObject::count != 0) {
		std::cout << "failed\n  Test Object count wrong, got "
							<< (unsigned int)TestObject::count << " objects."
							<< std::endl;
		result = 1;
	} else {
		std::cout << "passed" << std::endl;
	}
}

static ref global;

ref &get_reference() {
	return global;
}

void weak_ref(int &result) {
	std::cout << "=============================================================="
						<< "=================\n"
						<< "TEST SUITE: Weak References" << std::endl;
	ref ptr1 = new TestObject;
	ref ptr2 = ptr1.weak();

	std::cout << "TEST: Weak Reference... " << std::flush;
	if (ref_examine(result, ptr1, false, false, 1, 1, 1) and
			ref_examine(result, ptr2, false, true, 1, 1, 1)) {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Weak Reference to Null Object... " << std::flush;
	ptr1 = NULL;
	if (ref_examine(result, ptr1, true, false, 0, 0, 0) and
		  ref_examine(result, ptr2, true, true, 0, 1, 0)) {
		std::cout << "passed" << std::endl;
	}

	std::cout << "TEST: Weak Reference to Result Reference... " << std::flush;
	ptr1 = new TestObject;
	get_reference() = ptr1.weak();
	if (not get_reference().is_weak()) {
		std::cout << "failed" << std::endl;
	} else {
		std::cout << "passed" << std::endl;
	}
}

int main(int argc, char *argv[]) {
	int result = 0;

	basic_ref_counting(result);
	ref_replace(result);
	weak_ref(result);

	return result;
}
