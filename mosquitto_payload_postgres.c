/*
Copyright (c) 2020 Roger Light <roger@atchoo.org>
Copyright (c) 2024 Mr. Chip <mrchip.zxf@outlook.com>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
   Mr. Chip - implement store message to postgres.
*/

/*
 * This is an *example* plugin which demonstrates how to save the payload of
 * a message to postgres after it is received by the broker and before it is sent on to
 * other clients modified from mosquitto_payload_modification plugin.
 *
 * You should be very sure of what you are doing before making use of this feature.
 *
 * Compile with:
 *   gcc -I<path to mosquitto-repo/include> -fPIC -shared mosquitto_payload_modification.c -o mosquitto_payload_modification.so
 *
 * Use in config with:
 *
 *   plugin /path/to/mosquitto_payload_modification.so
 *
 * Note that this only works on Mosquitto 2.0 or later.
 */
#include <stdio.h>
#include <string.h>

#include "mosquitto_broker.h"
#include "mosquitto_plugin.h"
#include "mosquitto.h"
#include "mqtt_protocol.h"
#include <mosquitto_broker_internal.h>
#include <libpq-fe.h>

#define UNUSED(A) (void)(A)

static mosquitto_plugin_id_t *mosq_pid = NULL;
static PGconn *conn = NULL;
static int nParams = 4;

static int callback_message(int event, void *event_data, void *userdata)
{
	struct mosquitto_evt_message *ed = event_data;

	UNUSED(event);
	UNUSED(userdata);

	printf("username: %s\r\n", ed->client->username);	// is password?
	printf("client-name: %s\r\n", ed->client->id);
	printf("client-id: %s\r\n", ed->client->address);
	printf("topic: %s\r\n", ed->topic);
	printf("payload: %s\r\n", ed->payload);


    const char *const paramValues[] = {ed->client->id, ed->client->address, ed->topic, ed->payload };
    const int paramLengths[] = {strlen(ed->client->id), strlen(ed->client->address), strlen(ed->topic), ed->payloadlen};
    const int paramFormats[] = { 0, 0, 0, 0};
    int resultFormat = 0;


	PGresult* res = PQexecPrepared(conn, "insertStatement", nParams, paramValues, paramLengths,
									paramFormats, resultFormat);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Prepared failed: %s\r\n", PQresultErrorMessage(res));
		PQclear(res);
		return MOSQ_ERR_UNKNOWN;
	}
	PQclear(res);
    
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_plugin_version(int supported_version_count, const int *supported_versions)
{
	int i;

	for(i=0; i<supported_version_count; i++){
		if(supported_versions[i] == 5){
			return 5;
		}
	}
	return -1;
}

int mosquitto_plugin_init(mosquitto_plugin_id_t *identifier, void **user_data, struct mosquitto_opt *opts, int opt_count)
{
	UNUSED(user_data);

	int i;
	struct mosquitto_opt *o;

	char *host;
	char *port;
	char *dbname;
	char *user;
	char *password;
	char *tb_name;
	for (i = 0, o = opts; i < opt_count; ++i, o++) {
		printf("key: %s, value: %s\r\n", o->key, o->value);
		if (strcmp(o->key, "pg_host") == 0) {
			host = o->value;
		} else if (strcmp(o->key, "pg_port") == 0) {
			port = o->value;
		} else if (strcmp(o->key, "pg_dbname") == 0) {
			dbname = o->value;
		} else if (strcmp(o->key, "pg_user") == 0) {
			user = o->value;
		} else if (strcmp(o->key, "pg_password") == 0) {
			password = o->value;
		} else if (strcmp(o->key, "pg_table") == 0) {
			tb_name = o->value;
		}
	}

	char conn_info[256];
	snprintf(conn_info, 256, "host=%s port=%s dbname=%s user=%s password=%s", host, port, dbname, user, password);
    // const char *conninfo = "host=127.0.0.1 port=5432 dbname=mosquitto user=mosquitto password=mosquitto";
	printf("conn_info: %s\r\n", conn_info);
    conn = PQconnectdb(conn_info);
	if (PQstatus(conn) != CONNECTION_OK) {
		fprintf(stderr, "Connection to dabase failed: %s\r\n",
		PQerrorMessage(conn));
		PQfinish(conn);
		return MOSQ_ERR_UNKNOWN;
	} 


	char statement[512];
	snprintf(statement, 512, "insert into %s(username, clientId, topic, message) VALUES($1, $2, $3, $4);", tb_name);
	PGresult* res = PQprepare(conn, "insertStatement", statement, nParams, NULL);
	// PGresult* res = PQprepare(conn, "insertStatement", "insert into tb_msg(clientId, topic, message) VALUES($1, $2, $3);", nParams, NULL);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Prepare failed: %s\r\n", PQresultErrorMessage(res));
        PQclear(res);
		return MOSQ_ERR_UNKNOWN;
    }
	mosq_pid = identifier;
	return mosquitto_callback_register(mosq_pid, MOSQ_EVT_MESSAGE, callback_message, NULL, NULL);
}

int mosquitto_plugin_cleanup(void *user_data, struct mosquitto_opt *opts, int opt_count)
{
	UNUSED(user_data);
	UNUSED(opts);
	UNUSED(opt_count);
	PQfinish(conn);
	return mosquitto_callback_unregister(mosq_pid, MOSQ_EVT_MESSAGE, callback_message, NULL);
}
