/*                                                                  -*- c++ -*-
 * The Bytecode Interpreter
 *
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

#include <meat.h>
#include <meat/utilities.h>
#include <meat/bytecode.h>

#include <cstring>
#include <sstream>
#include <cmath>

#ifdef TESTING
#include <testing.h>
#endif

#ifdef DEBUG
#include <iostream>
#include <iomanip>
#include <cstddef>

inline std::string local(std::uint8_t id) {
  switch (id) {
  case 0:
    return "self";
  case 1:
    return "class";
  case 2:
    return "context";
  case 3:
    return "null";
  default: {
    std::stringstream ss;
    ss << "L" << (unsigned int)id;
    return ss.str();
  }
  }
}

#endif

using namespace meat;

/*****************
 * meat::execute *
 *****************/

meat::Reference meat::execute(Reference context) {

  if (context.is_null()) {
    throw Exception("Unable to execute a Null object");
  } else  if (cast<Context>(context).flags == Context::PRIMATIVE) {
    /* Here we execute a c++ native function */

    cast<Context>(context).local(2) = context.weak(); // context
    Reference result = cast<Context>(context).pointer(context);
    cast<Context>(context).result(result);

    return result;

  } else {
    /* The bytecode interpreter starts here */
    std::uint16_t ip = meat::cast<Context>(context)._ip;
    const std::uint8_t *code =
      cast<Class>(cast<Context>(context).klass()).bytecode();
    cast<Context>(context).local(2) = context.weak();

#ifdef DEBUG
    // Adds the class hash id and ip to debugging messages.
#define BCLOC "(" << (cast<Class>(cast<Context>(context).klass()).name()) \
                  << "," << itohex(ip, 4) << ")"
#endif

    while (not context.is_null()) {

      /* Execute the byte code.
       */
      bytecode::bytecode_t *bc = (bytecode::bytecode_t *)&code[ip];
      switch (bc->code) {

      case meat::bytecode::NOOP:
        // No Operations, possibly could be used for byte alignment.
        ip++;
        break;

      case meat::bytecode::MESG_SUPER:
      case meat::bytecode::MESSAGE: {
        // Send a message to an object.

#ifdef DEBUG
        Class &klass = cast<Class>(cast<Context>(context).klass());
        std::cout << "BC" << BCLOC;
        switch (bc->code) {
        case meat::bytecode::MESSAGE:
          std::cout << ": MESSAGE "; break;
        case meat::bytecode::MESG_SUPER:
          std::cout << ": MESSAGE SUPER "; break;
        default: break;
        }
        std::cout << local(bc->o.m.object) << " "
                  << klass.lookup(endian::read_be(bc->o.m.message_id)) << " ";
        if (bc->o.m.parameters) {
          for (uint8_t c = 0; c < bc->o.m.parameters; c++) {
            std::cout << " " << local(bc->o.m.parameter[c]);
          }
        }
        std::cout << std::endl;
#endif /* DEBUG */

        Reference obj = cast<Context>(context).local(bc->o.m.object);
        std::uint32_t method_id = endian::read_be(bc->o.m.message_id);

        // Create the new context in which to execute the message in.
        Reference new_ctx;
        if (code[ip] == meat::bytecode::MESSAGE)
          new_ctx = message(obj, method_id, context);
        else
          new_ctx = message_super(obj, method_id, context);

        // Add the parameters to the new context.
        for (uint8_t c = 0; c < bc->o.m.parameters; c++) {
          Reference param =
            cast<Context>(context).local(bc->o.m.parameter[c]);
          cast<Context>(new_ctx).parameter(c, param);
        }

        // Update the code pointer in the context.
        ip += 7 + bc->o.m.parameters;
        cast<Context>(context)._ip = ip;

        // Execute the message.
        if (cast<Context>(new_ctx).flags == Context::PRIMATIVE) {
          cast<Context>(new_ctx).local(2) = new_ctx.weak();
          cast<Context>(new_ctx).pointer(new_ctx);
        } else {
          context = new_ctx;
          ip = cast<Context>(context)._ip;
          code =
            cast<Class>(cast<Context>(context).klass()).bytecode();
          cast<Context>(context).local(2) = context.weak();
        }

        break;
      }

      case meat::bytecode::MESG_SUPER_RESULT:
      case meat::bytecode::MESG_RESULT: {
        // Send a message to an object with results.

#ifdef DEBUG
        Class &klass = cast<Class>(cast<Context>(context).klass());
        std::cout << "BC" << BCLOC;
        switch (bc->code) {
        case meat::bytecode::MESG_RESULT:
          std::cout << ": MESSAGE RESULT "; break;
        case meat::bytecode::MESG_SUPER:
          std::cout << ": MESSAGE SUPER RESULT "; break;
        default: break;
        }
        std::cout << local(bc->o.mr.result) << " = "
                  << local(bc->o.mr.object) << " "
                  << klass.lookup(endian::read_be(bc->o.mr.message_id)) << " ";
        if (bc->o.mr.parameters) {
          for (uint8_t c = 0; c < bc->o.mr.parameters; c++) {
            std::cout << local(bc->o.mr.parameter[c]) << " ";
          }
        }
        std::cout << std::endl;
#endif /* DEBUG */

        Reference obj = cast<Context>(context).local(bc->o.mr.object);
        std::uint32_t method_id = endian::read_be(bc->o.mr.message_id);

        // Create the new context in which to execute the message in.
        Reference new_ctx;
        if (code[ip] == meat::bytecode::MESG_RESULT)
          new_ctx = message(obj, method_id, context);
        else
          new_ctx = message_super(obj, method_id, context);

        // Add the parameters to the new context.
        for (uint8_t c = 0; c < bc->o.mr.parameters; c++) {
          Reference param =
            cast<Context>(context).local(bc->o.mr.parameter[c]);
          cast<Context>(new_ctx).parameter(c, param);
        }

        // Update the code pointer in the context.
        ip += 8 + bc->o.mr.parameters;
        cast<Context>(context)._ip = ip;

        cast<Context>(new_ctx).result_index(bc->o.mr.result);

        // Execute the message.
        if (cast<Context>(new_ctx).flags == Context::PRIMATIVE) {
          cast<Context>(new_ctx).local(2) = new_ctx.weak();
          Reference result = cast<Context>(new_ctx).pointer(new_ctx);
          cast<Context>(new_ctx).result(result);
        } else {
          context = new_ctx;
          ip = cast<Context>(context)._ip;
          code = cast<Class>(cast<Context>(context).klass()).bytecode();
          cast<Context>(context).local(2) = context.weak();
        }

        break;
      }

      case meat::bytecode::BLOCK: {
#ifdef DEBUG
        std::cout << "BC" << BCLOC << ": BLOCK CONTEXT "
                  << std::dec
                  << local(bc->o.bc.result) << " "
                  << (unsigned int)bc->o.bc.locals << " "
                  << (unsigned int)endian::read_be(bc->o.bc.code_size)
                  << std::endl;
#endif /* DEBUG */

        Reference block = new BlockContext(context, bc->o.bc.locals, ip + 5);
        cast<Context>(context).local(bc->o.bc.result) = block;

        ip += endian::read_be(bc->o.bc.code_size) + 5;
        cast<Context>(context)._ip = ip;
        break;
      }

      case meat::bytecode::CONTEXT_END:
#ifdef DEBUG
        std::cout << "BC" << BCLOC << ": CONTEXT END"
                  << std::endl;
#endif /* DEBUG */
        cast<Context>(context).finish();
        break;

      case meat::bytecode::ASSIGN: {
#ifdef DEBUG
        std::cout << "BC" << BCLOC << ": ASSIGN " << std::dec
                  << local(bc->o.a.destination) << " = "
                  << local(bc->o.a.source)
                  << std::endl;
#endif /* DEBUG */
        Reference src =
          cast<Context>(context).local(bc->o.a.source);
        cast<Context>(context).local(bc->o.a.destination) = src;
        ip += 3;
        break;
      }

      case meat::bytecode::ASSIGN_PROP:
      case meat::bytecode::ASSIGN_CLASS_PROP: {
        Reference self;
        if (bc->code == bytecode::ASSIGN_PROP)
          self = cast<Context>(context).self();
        else
          self = cast<Context>(context).klass();

#ifdef DEBUG
        std::cout << "BC" << BCLOC;
        switch (bc->code) {
        case bytecode::ASSIGN_PROP:
          std::cout << ": GET PROPERTY "; break;
        case bytecode::ASSIGN_CLASS_PROP:
          std::cout << ": GET CLASS PROPERTY "; break;
        default: break;
        }
        std::cout << local(bc->o.ap.destination) << " = "
                  << (unsigned int)bc->o.ap.property_id
                  << std::endl;
#endif /* DEBUG */
        cast<Context>(context).local(bc->o.ap.destination) =
          self->property(bc->o.ap.property_id);
        ip += 3;
        break;
      }

      case meat::bytecode::ASSIGN_CLASS: {
#ifdef DEBUG
        Class &klass = cast<Class>(cast<Context>(context).klass());
        std::cout << "BC" << BCLOC << ": GET CLASS " << std::dec
                  << local(bc->o.c.destination) << " = "
                  << klass.lookup(endian::read_be(bc->o.c.class_id))
                  << std::endl;
#endif /* DEBUG */

        cast<Context>(context).local(bc->o.c.destination) =
          Class::resolve(endian::read_be(bc->o.c.class_id));
        ip += 6;
        break;
      }

      case meat::bytecode::ASSIGN_CONST_INT: {
#ifdef DEBUG
        std::cout << "BC" << BCLOC << ": INTEGER " << std::dec
                  << local(bc->o.ci.destination) << " = "
                  << (int)endian::read_be(bc->o.ci.value)
                  << std::endl;
#endif /* DEBUG */

        Reference intobj = new Value(endian::read_be(bc->o.ci.value));
        cast<Context>(context).local(bc->o.ci.destination) = intobj;
        ip += 6;
        break;
      }

      case meat::bytecode::ASSIGN_CONST_NUM: {
#ifdef DEBUG
        std::cout << "BC" << BCLOC << ": NUMBER " << std::dec
                  << local(bc->o.cn.destination) << " = ";
#endif /* DEBUG */

        double mantissa = ldexp(endian::read_be(bc->o.cn.mantissa), -53);
        Reference float_obj =
          new Value(ldexp(mantissa, endian::read_be(bc->o.cn.exponent)));
        cast<Context>(context).local(bc->o.cn.destination) = float_obj;
#ifdef DEBUG
        std::cout << FLOAT(float_obj) << std::endl;
#endif /* DEBUG */
        ip += 13;
        break;
      }

      case meat::bytecode::ASSIGN_CONST_TXT: {
#ifdef DEBUG
        std::cout << "BC" << BCLOC << ": TEXT " << std::dec
                  << local(bc->o.ct.destination) << " = "
                  << "\"" << bc->o.ct.value << "\""
                  << std::endl;
#endif /* DEBUG */

        Reference strobj = new Text((const char *)bc->o.ct.value);
        cast<Context>(context).local(bc->o.ct.destination) = strobj;
        ip += strlen((const char *)bc->o.ct.value) + 3;
        break;
      }

      case meat::bytecode::SET_PROP:
      case meat::bytecode::SET_CLASS_PROP: {
#ifdef DEBUG
        std::cout << "BC" << BCLOC;
        switch (bc->code) {
        case bytecode::SET_PROP:
          std::cout << ": SET PROPERTY "; break;
        case bytecode::SET_CLASS_PROP:
          std::cout << ": SET CLASS PROPERTY "; break;
        default: break;
        }
        std::cout << (unsigned int)bc->o.sp.destination << " = "
                  << local(bc->o.sp.source)
                  << std::endl;
#endif /* DEBUG */

        Reference self;
        if (bc->code == bytecode::SET_PROP)
          self = cast<Context>(context).self();
        else
          self = cast<Context>(context).klass();
        self->property(bc->o.sp.destination) =
          cast<Context>(context).local(bc->o.sp.source);
        ip += 3;
        break;
      }

      default: {
        std::stringstream errmsg;
        errmsg << "Unknown bytecode " << std::showbase << std::hex
               << (unsigned int)code[ip] << " @ " << (unsigned int)ip
               << " in class "
               << cast<Class>(cast<Context>(context).klass()).name();
        throw Exception(errmsg.str());
      }
      }

      /*  Now we need to check if the current context is done and if it is
       * look for the next upper level not done context.
       */
      while (cast<Context>(context).is_done()) {
        /*  Get the result and the messaging context to the current context.
         * We also clean up the reference to the messaging context in the old
         * context. This ensures when a block context is kept that only
         * necessary contexts are kept.
         */
#ifdef TESTING
        meat::test::test("Context weak self reference", false);
        if (not cast<Context>(context).local(2).is_weak()) {
          meat::test::failed("Context weak self reference", false);
        }
#endif
        Reference old_ctx = context;
        context = cast<Context>(context).messenger();
        cast<Context>(old_ctx).messenger(meat::Null());
        //cast<Context>(old_ctx).local(2) = NULL; // context

        // If we are the top level context then return.
        if (context.is_null() or context == meat::Null()) {
          return cast<Context>(old_ctx).result();
        }

        /*  If we were called by a primative method then we need to return back
         * to it.
         */
        if (cast<Context>(context).flags == Context::PRIMATIVE) {
          return cast<Context>(old_ctx).result();
        }

        cast<Context>(old_ctx).reset();

        if (not cast<Context>(context).is_done()) {
          ip = cast<Context>(context)._ip;
          code = cast<Class>(cast<Context>(context).klass()).bytecode();
          cast<Context>(context).local(2) = context.weak(); // context
        }
      }
    }
  }

  return meat::Null();
}
