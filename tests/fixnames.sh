#! /bin/bash

#  Since meat doesn't allow to replace loaded libraries and classes we add
# the ᶲ symbol to the end of the names. This script goes in afterwards and
# fixes all the names and hashes in the cpp files.

FILE="${1}"

mv "${FILE}" "${FILE}.tmp"
cat "${FILE}.tmp" | sed -e 's/ᶲ//g' > "${FILE}"
rm "${FILE}.tmp"
shift

NAMES=
case "${FILE}" in
		builtin.cpp)
				NAMES="Object Class Context BlockContext Null Exception Boolean \
						   Numeric Integer Number Text List Set Index Application"
				;;
		grinder.cpp)
				NAMES="Grinder.SyntaxException Grinder.Library Grinder.Class \
               Grinder.Method"
				;;
		Datastore.cpp)
				NAMES="Library Archive"
				;;
esac

for NAME in ${NAMES}; do
		WRONG=$(../bin/meat-grinder -# "${NAME}ᶲ")
		RIGHT=$(../bin/meat-grinder -# "${NAME}")

		mv "${FILE}" "${FILE}.tmp"
		cat "${FILE}.tmp" | sed -e "s/${WRONG}/${RIGHT}/g" > "${FILE}"
		rm "${FILE}.tmp"
done
