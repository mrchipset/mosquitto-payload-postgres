include ../../config.mk

.PHONY : all binary check clean reallyclean test install uninstall

PLUGIN_NAME=mosquitto_payload_postgres

all : binary target

target:
	-cp ${PLUGIN_NAME}.so /etc/mosquitto/conf.d

binary : ${PLUGIN_NAME}.so

${PLUGIN_NAME}.so : ${PLUGIN_NAME}.c
	$(CROSS_COMPILE)$(CC) $(PLUGIN_CPPFLAGS)  $(PLUGIN_CFLAGS) $(PLUGIN_LDFLAGS) -g -fPIC -shared $< -o $@ -lpq

reallyclean : clean
clean:
	-rm -f *.o ${PLUGIN_NAME}.so *.gcda *.gcno

check: test
test:

install: ${PLUGIN_NAME}.so
	# Don't install, these are examples only.
	#$(INSTALL) -d "${DESTDIR}$(libdir)"
	#$(INSTALL) ${STRIP_OPTS} ${PLUGIN_NAME}.so "${DESTDIR}${libdir}/${PLUGIN_NAME}.so"

uninstall :
	-rm -f "${DESTDIR}${libdir}/${PLUGIN_NAME}.so"
