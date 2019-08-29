/******************************* OTA Controller *******************************
This file is part of the Ewings Esp8266 Stack.

This is free software. you can redistribute it and/or modify it but without any
warranty.

Author          : Suraj I.
created Date    : 1st June 2019
******************************************************************************/

#ifndef _EW_SERVER_OTA_CONTROLLER_
#define _EW_SERVER_OTA_CONTROLLER_

#include <webserver/resources/WebResource.h>
#include <webserver/pages/WiFiConfigPage.h>
#include <webserver/pages/OtaConfigPage.h>

/**
 * OtaController class
 */
class OtaController {

	protected:

		/**
		 * @var	EwWebResourceProvider*	web_resource
		 */
		EwWebResourceProvider* web_resource;

	public:

		/**
		 * OtaController constructor
		 */
		OtaController(){
		}

		/**
		 * OtaController destructor
		 */
		~OtaController(){
		}

		/**
		 * register ota route handler
		 *
		 * @param	EwWebResourceProvider*	_web_resource
		 */
		void handle( EwWebResourceProvider* _web_resource ){

			this->web_resource = _web_resource;
			this->web_resource->register_route( EW_SERVER_OTA_CONFIG_ROUTE, [&]() { this->handleOtaServerConfigRoute(); }, AUTH_MIDDLEWARE );
		}

		/**
		 * build ota server config html.
		 *
		 * @param	char*	_page
		 * @param	bool|false	_enable_flash
		 * @param	int|EW_HTML_MAX_SIZE	_max_size
		 */
		void build_ota_server_config_html( char* _page, bool _enable_flash=false, int _max_size=EW_HTML_MAX_SIZE ){

      memset( _page, 0, _max_size );
      strcat_P( _page, EW_SERVER_HEADER_HTML );
      strcat_P( _page, EW_SERVER_OTA_CONFIG_PAGE_TOP );

      ota_config_table _ota_configs = this->web_resource->ew_db->get_ota_config_table();

      char _port[10];memset( _port, 0, 10 );
      __appendUintToBuff( _port, "%d", _ota_configs.ota_port, 8 );

      concat_tr_input_html_tags( _page, PSTR("OTA Host:"), PSTR("hst"), _ota_configs.ota_host, OTA_HOST_BUF_SIZE-1 );
      concat_tr_input_html_tags( _page, PSTR("OTA Port:"), PSTR("prt"), _port );

      strcat_P( _page, EW_SERVER_WIFI_CONFIG_PAGE_BOTTOM );
      if( _enable_flash )
      concat_flash_message_div( _page, HTML_SUCCESS_FLASH, ALERT_SUCCESS );
      strcat_P( _page, EW_SERVER_FOOTER_HTML );
    }

		/**
		 * build and send ota server config page.
		 * when posted, get ota server configs from client and set them in database.
		 */
    void handleOtaServerConfigRoute( void ) {
      #ifdef EW_SERIAL_LOG
      Logln(F("Handling OTA Server Config route"));
      #endif
      bool _is_posted = false;

      if ( this->web_resource->EwServer->hasArg("hst") && this->web_resource->EwServer->hasArg("prt") ) {

        String _ota_host = this->web_resource->EwServer->arg("hst");
        String _ota_port = this->web_resource->EwServer->arg("prt");

        #ifdef EW_SERIAL_LOG
          Logln(F("\nSubmitted info :\n"));
          Log(F("ota host : ")); Logln( _ota_host );
          Log(F("ota port : ")); Logln( _ota_port );
          Logln();
        #endif

        ota_config_table _ota_configs = this->web_resource->ew_db->get_ota_config_table();

        _ota_host.toCharArray( _ota_configs.ota_host, _ota_host.length()+1 );
        _ota_configs.ota_port = (int)_ota_port.toInt();

        this->web_resource->ew_db->set_ota_config_table( &_ota_configs);

        _is_posted = true;
      }

      char* _page = new char[EW_HTML_MAX_SIZE];
      this->build_ota_server_config_html( _page, _is_posted );

      this->web_resource->EwServer->send( HTTP_OK, EW_HTML_CONTENT, _page );
      delete[] _page;
    }

};

#endif