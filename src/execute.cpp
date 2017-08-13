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
#ifdef DEBUG
#include <iostream>
#include <iomanip>

inline std::string local(meat::uint8_t id) {
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

/*****************
 * meat::execute *
 *****************/

meat::Reference meat::execute(Reference context) {

	if (context.is_null()) {
		throw Exception("Unable to execute a Null object");
	} else	if (CONTEXT(context).flags == meat::Context::PRIMATIVE) {
		/* Here we execute a c++ native function */

		CONTEXT(context).set_local(2, context.weak()); // context
		Reference result = CONTEXT(context).pointer(context);
		CONTEXT(context).set_result(result);

		return result;

	} else {
		/* The bytecode interpreter starts here */
		meat::uint16_t ip = CONTEXT(context).ip;
		const uint8_t *code = CLASS(CONTEXT(context).get_class()).get_bytecode();
		CONTEXT(context).set_local(2, context.weak()); // context

#ifdef DEBUG
		// Adds the class hash id and ip to debugging messages.
#define BCLOC "(" << (CLASS(CONTEXT(context).get_class()).get_name()) \
                  << "," << itohex(ip, 4) << ")"
#endif

		while (not context.is_null()) {

			/* Execute the byte code.
			 */
			switch (code[ip]) {

			case meat::bytecode::NOOP:
				// No Operations, possibly could be used for byte alignment.
				ip += 1;
				break;

			case meat::bytecode::MESSAGE: {
				/* Send a message to an object.
				 * MESSAGE obj mesg_id param_cnt params...
				 */
				uint32_t *mesg_id = (uint32_t *)&code[ip + 2];
				uint8_t params = code[ip + 6];

#ifdef DEBUG
				Class &klass = CLASS(CONTEXT(context).get_class());
				std::cout << "BC" << BCLOC << ": MESSAGE " << std::dec
									<< local(code[ip + 1]) << " "
									<< klass.lookup(endian::read_be(*mesg_id)) << " ";
					if (params) {
						for (uint8_t c = 0; c < params; c++) {
							std::cout << " " << local(code[ip + 7 + c]);
						}
					}
				std::cout << std::endl;
#endif /* DEBUG */

				Reference obj = CONTEXT(context).get_local(code[ip + 1]);
				meat::uint32_t method_id = endian::read_be(*mesg_id);

				// Create the new context in which to execute the message in.
				Reference new_ctx = message(obj, method_id, context);

				// Add the parameters to the new context.
				for (uint8_t c = 0; c < params; c++) {
					Reference param = CONTEXT(context).get_local(code[ip + 7 + c]);
					CONTEXT(new_ctx).set_param(c, param);
				}

				// Update the code pointer in the context.
				ip += 7 + params;
				CONTEXT(context).ip = ip;

				// Execute the message.
				if (CONTEXT(new_ctx).flags == meat::Context::PRIMATIVE) {
					CONTEXT(new_ctx).set_local(2, new_ctx.weak()); // context
					Reference result = CONTEXT(new_ctx).pointer(new_ctx);
					CONTEXT(new_ctx).set_result(result);
				} else {
					context = new_ctx;
					ip = CONTEXT(context).ip;
					code = CLASS(CONTEXT(context).get_class()).get_bytecode();
					CONTEXT(context).set_local(2, context.weak()); // context
				}

				break;
			}

			case meat::bytecode::MESG_RESULT: {
				/* Send a message to an object.
				 * MESSAGE obj result_idx mesg_id param_cnt params...
				 */
				uint8_t result_idx = code[ip + 2];
				uint32_t *mesg_id = (uint32_t *)&code[ip + 3];
				uint8_t params = code[ip + 7];

#ifdef DEBUG
				Class &klass = CLASS(CONTEXT(context).get_class());
				std::cout << "BC" << BCLOC << ": MSGRES " << std::dec
									<< local(code[ip + 1]) << " "
									<< local(result_idx) << " "
									<< klass.lookup(endian::read_be(*mesg_id)) << " ";
				if (params) {
					//std::cout << "(";
					for (uint8_t c = 0; c < params; c++) {
						std::cout << local(code[ip + 8 + c]) << " ";
					}
					//std::cout << ")";
				}
				std::cout << std::endl;
#endif /* DEBUG */

				Reference obj = CONTEXT(context).get_local(code[ip + 1]);
				meat::uint32_t method_id = endian::read_be(*mesg_id);

				// Create the new context in which to execute the message in.
				Reference new_ctx = message(obj, method_id, context);

				// Add the parameters to the new context.
				for (uint8_t c = 0; c < params; c++) {
					Reference param = CONTEXT(context).get_local(code[ip + 8 + c]);
					CONTEXT(new_ctx).set_param(c, param);
				}

				// Update the code pointer in the context.
				ip += 8 + params;
				CONTEXT(context).ip = ip;

				CONTEXT(new_ctx).set_result_index(result_idx);
				// Execute the message.
				if (CONTEXT(new_ctx).flags == meat::Context::PRIMATIVE) {
					CONTEXT(new_ctx).set_local(2, new_ctx.weak()); // context
					Reference result = CONTEXT(new_ctx).pointer(new_ctx);
					CONTEXT(new_ctx).set_result(result);
				} else {
					context = new_ctx;
					ip = CONTEXT(context).ip;
					code = CLASS(CONTEXT(context).get_class()).get_bytecode();
				}
				CONTEXT(context).set_local(2, context.weak()); // context

				break;
			}

			case meat::bytecode::MESG_SUPER: {
				/* Send a message to an object.
				 * SUPER obj mesg_id param_cnt params...
				 */
				uint32_t *mesg_id = (uint32_t *)&code[ip + 2];
				uint8_t params = code[ip + 6];

#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": SUPER " << std::dec
									<< local(code[ip + 1]) << " "
									<< std::hex << endian::read_be(*mesg_id) << " "
									<< std::dec << (unsigned int)params
									<< std::endl;
#endif /* DEBUG */

				Reference obj = CONTEXT(context).get_local(code[ip + 1]);
				meat::uint32_t method_id = endian::read_be(*mesg_id);

				// Create the new context in which to execute the message in.
				Reference new_ctx = message_super(obj, method_id, context);

				// Add the parameters to the new context.
				for (uint8_t c = 0; c < params; c++) {
					Reference param = CONTEXT(context).get_local(code[ip + 7 + c]);
					CONTEXT(new_ctx).set_param(c, param);
				}

				// Update the code pointer in the context.
				ip += 7 + params;
				CONTEXT(context).ip = ip;

				// Execute the message.
				if (CONTEXT(new_ctx).flags == meat::Context::PRIMATIVE) {
					CONTEXT(new_ctx).set_local(2, new_ctx.weak()); // context
					Reference result = CONTEXT(new_ctx).pointer(new_ctx);
					CONTEXT(new_ctx).set_result(result);
				} else {
					context = new_ctx;
					ip = CONTEXT(context).ip;
					code = CLASS(CONTEXT(context).get_class()).get_bytecode();
				}
				CONTEXT(context).set_local(2, context.weak()); // context

				break;
			}

			case meat::bytecode::MESG_SUPER_RESULT: {
				/* Send a message to an object.
				 * SUPER_RES obj result_idx mesg_id param_cnt params...
				 */
				uint8_t result_idx = code[ip + 2];
				uint32_t *mesg_id = (uint32_t *)&code[ip + 3];
				uint8_t params = code[ip + 7];

#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": SUPERRES " << std::dec
									<< local(code[ip + 1]) << " "
									<< local(result_idx) << " "
									<< std::hex << endian::read_be(*mesg_id) << " "
									<< std::dec << (unsigned int)params
									<< std::endl;
#endif /* DEBUG */

				Reference obj = CONTEXT(context).get_local(code[ip + 1]);
				meat::uint32_t method_id = endian::read_be(*mesg_id);

				// Create the new context in which to execute the message in.
				Reference new_ctx = message_super(obj, method_id, context);

				// Add the parameters to the new context.
				for (uint8_t c = 0; c < params; c++) {
					Reference param = CONTEXT(context).get_local(code[ip + 8 + c]);
					CONTEXT(new_ctx).set_param(c, param);
				}

				// Update the code pointer in the context.
				ip += 8 + params;
				CONTEXT(context).ip = ip;

				CONTEXT(new_ctx).set_result_index(result_idx);
				// Execute the message.
				if (CONTEXT(new_ctx).flags == meat::Context::PRIMATIVE) {
					CONTEXT(new_ctx).set_local(2, new_ctx.weak()); // context
					Reference result = CONTEXT(new_ctx).pointer(new_ctx);
					CONTEXT(new_ctx).set_result(result);
				} else {
					context = new_ctx;
					ip = CONTEXT(context).ip;
					code = CLASS(CONTEXT(context).get_class()).get_bytecode();
				}
				CONTEXT(context).set_local(2, context.weak()); // context

				break;
			}

			case meat::bytecode::BLOCK: {
				uint8_t local_id = code[ip + 1];
				uint8_t locals = code[ip + 2];
				uint16_t *block_size = (uint16_t *)&code[ip + 3];

#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": BLOCK " << std::dec
									<< (unsigned int)local_id << " "
									<< (unsigned int)locals << " "
									<< (unsigned int)endian::read_be(*block_size)
									<< std::endl;
#endif /* DEBUG */

				Reference block = new BlockContext(context, locals, ip + 5);
				CONTEXT(context).set_local(local_id, block);

				ip += endian::read_be(*block_size) + 5;
				CONTEXT(context).ip = ip;
				break;
			}

			case meat::bytecode::CONTEXT_END:
#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": CTXEND" << std::endl;
#endif /* DEBUG */
				CONTEXT(context).finish();
				break;

			case meat::bytecode::ASSIGN: {
#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": MOV " << std::dec
									<< local(code[ip + 1]) << " "
									<< local(code[ip + 2])
									<< std::endl;
#endif /* DEBUG */
				Reference src = CONTEXT(context).get_local(code[ip + 2]);
				CONTEXT(context).set_local(code[ip + 1], src);
				ip += 3;
				break;
			}

			case meat::bytecode::ASSIGN_OATTR: {
				Reference self = CONTEXT(context).get_self();

#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": GETATTR " << std::dec
									<< local(code[ip + 1]) << " "
									<< (unsigned int)code[ip + 2]
									<< std::endl;
#endif /* DEBUG */

				CONTEXT(context).set_local(code[ip + 1], self->property(code[ip + 2]));
				ip += 3;
				break;
			}

			case meat::bytecode::ASSIGN_CATTR: {
				Reference self = CONTEXT(context).get_class();
#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": GETCATTR " << std::dec
									<< local(code[ip + 1]) << " "
									<< (unsigned int)code[ip + 2]
									<< std::endl;
#endif /* DEBUG */
				CONTEXT(context).set_local(code[ip + 1], self->property(code[ip + 2]));
				ip += 3;
				break;
			}

			case meat::bytecode::ASSIGN_CLASS: {
				uint8_t local_id = code[ip + 1];
				uint32_t *class_id = (uint32_t *)&code[ip + 2];

#ifdef DEBUG
				Class &klass = CLASS(CONTEXT(context).get_class());
				std::cout << "BC" << BCLOC << ": CLASS " << std::dec
									<< local(code[ip + 1]) << " "
									<< klass.lookup(endian::read_be(*class_id))
									<< std::endl;
#endif /* DEBUG */

				CONTEXT(context).set_local(local_id,
																	 Class::resolve(endian::read_be(*class_id)));
				ip += 6;
				break;
			}

			case meat::bytecode::ASSIGN_CONST_INT: {
				uint8_t local_id = code[ip + 1];
				int32_t *i = (int32_t *)&code[ip + 2];

				Reference intobj = new Value(endian::read_be(*i));

#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": INT " << std::dec
									<< local(code[ip + 1]) << " "
									<< (int)endian::read_be(*i)
									<< std::endl;
#endif /* DEBUG */

				CONTEXT(context).set_local(local_id, intobj);
				ip += 6;
				break;
			}

			case meat::bytecode::ASSIGN_CONST_FLT: {
				uint8_t local_id = code[ip + 1];
				float_t *i = (float_t *)&code[ip + 2];

				Reference float_obj = new Value(endian::read_be(*i));

#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": FLOAT " << std::dec
									<< local(local_id) << " "
									<< (float)endian::read_be(*i)
									<< std::endl;
#endif /* DEBUG */

				CONTEXT(context).set_local(local_id, float_obj);
				ip += 6;
				break;
			}

			case meat::bytecode::ASSIGN_CONST_STR: {
				uint8_t local_id = code[ip + 1];
				const char *str = (const char *)&code[ip + 2];
				Reference strobj = new Text(str);

#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": STR " << std::dec
									<< local(code[ip + 1]) << " "
									<< "\"" << str << "\""
									<< std::endl;
#endif /* DEBUG */

				CONTEXT(context).set_local(local_id, strobj);
				ip += strlen(str) + 3;
				break;
			}

			case meat::bytecode::SET_OATTR: { // Set an object property
				uint8_t attr_id = code[ip + 1];
				uint8_t local_id = code[ip + 2];

#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": SETATTR " << std::dec
									<< (unsigned int)attr_id << " "
									<< local(local_id)
									<< std::endl;
#endif /* DEBUG */

				Reference self = CONTEXT(context).get_self();

				self->property(attr_id) = CONTEXT(context).get_local(local_id);
				ip += 3;
				break;
			}

			case meat::bytecode::SET_CATTR: { // Set an class property
				uint8_t attr_id = code[ip + 1];
				uint8_t local_id = code[ip + 2];

#ifdef DEBUG
				std::cout << "BC" << BCLOC << ": SETCATTR " << std::dec
									<< (unsigned int)attr_id << " "
									<< local(local_id)
									<< std::endl;
#endif /* DEBUG */

				Reference cls = CONTEXT(context).get_class();

				cls->property(attr_id) = CONTEXT(context).get_local(local_id);
				ip += 3;
				break;
			}

			default: {
				std::stringstream errmsg;
				errmsg << "Unknown bytecode " << std::showbase << std::hex
							 << (unsigned int)code[ip] << " @ " << (unsigned int)ip
							 << " in class id "
							 << (unsigned int)CLASS(CONTEXT(context).get_class()).get_hash_id();
				throw Exception(errmsg.str());
			}
			}

			/*  Now we need to check if the current context is done and if it is
			 * look for the next upper level not done context.
			 */
			while (CONTEXT(context).is_done()) {
				/*  Get the result and the messaging context to the current context.
				 * We also clean up the reference to the messaging context in the old
				 * context. This ensures when a block context is kept that only
				 * necessary contexts are kept.
				 */
				Reference result = CONTEXT(context).get_result();
				Reference old_ctx = context;
				context = CONTEXT(context).get_messenger();
				CONTEXT(old_ctx).set_messenger(meat::Null());

				// If we are the top level context then return.
				if (context.is_null() || context == meat::Null()) {
					return result;
				}

				/*  If we were called by a primative method then we need to return back
				 * to it.
				 */
				if (CONTEXT(context).flags == meat::Context::PRIMATIVE) {
					return result;
				}

				ip = CONTEXT(context).ip;
				code = CLASS(CONTEXT(context).get_class()).get_bytecode();
				CONTEXT(context).set_local(2, context.weak()); // context
			}
		}
	}

	return meat::Null();
}
