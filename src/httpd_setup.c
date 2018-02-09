/*####COPYRIGHTBEGIN####
 -------------------------------------------
Copyright (c) 2017 Brad L.

THE FOLLOWING LICENSE APPLIES ONLY TO FILES IN THE esp8266-BACnet/src FOLDER.
SEE OTHER FILES FOR LICENSE TERMS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 -------------------------------------------
####COPYRIGHTEND####*/

/*
	This file contains setup routines for the HTTP server
*/

#include "osapi.h"
#include "c_types.h"
#include "platform.h"
#include "httpd.h"
#include "httpdespfs.h"

HttpdBuiltInUrl builtInUrls[]={
	//{"/", cgiRedirect, "/index.cgi"},
	//{"*", cgiGreetUser, NULL},
	//{"*", cgiEspFsHook, NULL},
	{NULL, NULL, NULL}
};

int ICACHE_FLASH_ATTR cgiGreetUser(HttpdConnData *connData) {
	int len;			//length of user name
	char name[128];		//Temporary buffer for name
	char output[256];	//Temporary buffer for HTML output
	
	//If the browser unexpectedly closes the connection, the CGI will be called 
	//with connData->conn=NULL. We can use this to clean up any data. It's not really
	//used in this simple CGI function.
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	if (connData->requestType!=HTTPD_METHOD_GET) {
		//Sorry, we only accept GET requests.
		httpdStartResponse(connData, 406);  //http error code 'unacceptable'
		httpdEndHeaders(connData);
		return HTTPD_CGI_DONE;
	}

	//Look for the 'name' GET value. If found, urldecode it and return it into the 'name' var.
	//len=httpdFindArg(connData->getArgs, "name", name, sizeof(name));
	len = -1;
	if (len==-1) {
		//If the result of httpdFindArg is -1, the variable isn't found in the data.
		strcpy(name, "unknown person");
	} else {
		//If len isn't -1, the variable is found and is copied to the 'name' variable
	}
	
	//Generate the header
	//We want the header to start with HTTP code 200, which means the document is found.
	httpdStartResponse(connData, 200); 
	//We are going to send some HTML.
	httpdHeader(connData, "Content-Type", "text/html");
	//No more headers.
	httpdEndHeaders(connData);
	
	//We're going to send the HTML as two pieces: a head and a body. We could've also done
	//it in one go, but this demonstrates multiple ways of calling httpdSend.
	//Send the HTML head. Using -1 as the length will make httpdSend take the length
	//of the zero-terminated string it's passed as the amount of data to send.
	httpdSend(connData, "<html><head><title>Page</title></head>", -1);
	//Generate the HTML body. 
	len=sprintf(output, "<body><p>Hello, %s!</p></body></html>", name);
	//Send HTML body to webbrowser. We use the length as calculated by sprintf here.
	//Using -1 again would also have worked, but this is more efficient.
	httpdSend(connData, output, len);

	//All done.
	return HTTPD_CGI_DONE;
}

void ICACHE_FLASH_ATTR httpd_setup() {
	
	httpdInit(builtInUrls, 80);
}