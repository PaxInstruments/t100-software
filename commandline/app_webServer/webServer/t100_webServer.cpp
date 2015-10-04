/*-----------------------------------------------------------------------------
/
/
/
/----------------------------------------------------------------------------*/
#include <vector>
#include <mutex>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "hidapi.h"
#include <unistd.h>
#include "t100_lib.h"
#include "./mongoose/mongoose.h"
/*---------------------------------------------------------------------------*/
using namespace std;
/*---------------------------------------------------------------------------*/
int t100_count;
std::mutex mtx;
int deviceCount;
t100 t100_coordinator;
vector<t100*> t100_list;
/*---------------------------------------------------------------------------*/
t100* createT100();
void *connectionHandler(void *arg);
void t100Helper_periodicUpdate(vector<t100*> &deviceList);
void t100Helper_fillDeviceList(vector<t100*> &deviceList);
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data);
/*---------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{   
  struct mg_mgr mgr;
  pthread_t client_thread;
  struct mg_connection* nc;
  static const char *s_http_port = "8000";

  if(hid_init())
  {
    return -1;
  }

  /* Do the initial scan */
  t100Helper_fillDeviceList(t100_list);
    
  /* Web server initialisation */
  mg_mgr_init(&mgr, NULL);
  nc = mg_bind(&mgr, s_http_port, ev_handler);
  mg_set_protocol_http_websocket(nc);  
  printf("> Starting web server on port %s\n", s_http_port);

  /* Assign the client to its thread */
  if(pthread_create(&client_thread,NULL,connectionHandler,&mgr) < 0)
  {
    printf("> Thread creation error ...\n");
    return -1;
  }
  printf("> Client handler is assigned!\n");

  /* Do the dance ... */
  while(1)
  {      
    usleep(500 * 1000);
    
    mtx.lock();
    t100Helper_periodicUpdate(t100_list);
    mtx.unlock();
  }

  return 0; 
}
/*---------------------------------------------------------------------------*/
t100* createT100()
{
    t100* pt = (t100*) malloc(sizeof(t100));

    return pt;
}
/*---------------------------------------------------------------------------*/
void t100Helper_periodicUpdate(vector<t100*> &deviceList)
{
    for(int i=0;i<deviceList.size();i++)
    {
        deviceList.at(i)->periodicUpdate();
    }
}
/*---------------------------------------------------------------------------*/
void t100Helper_fillDeviceList(vector<t100*> &deviceList)
{
    int rval;
    int loopCount;
    int errorCount = 0;

    loopCount = deviceList.size();

    for(int i=0;i<loopCount;i++)
    {
        t100* tmp = deviceList.at(0);

        tmp->disconnect();

        deviceList.erase(deviceList.begin());
    }

    /* Fill the device count in a global array for later disconnect and all. */
    deviceCount = t100_coordinator.searchDevices();    

    for(int i=0;i<deviceCount;i++)
    {
        t100* tmp = createT100();

        tmp->init();

        rval = tmp->connectBySerial(t100_coordinator.getSerialNumber(i));

        if(rval < 0)
        {
            delete tmp;
            errorCount++;
        }
        else
        {            
            /* Default is K thermocouple ... */
            tmp->setThermocoupleType(KType);
            tmp->setPgaGain(8);
            deviceList.push_back(tmp);
        }                
    }

    deviceCount -= errorCount;
    t100_count = deviceCount;
}
/*---------------------------------------------------------------------------*/
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) 
{
  struct http_message *hm = (struct http_message *) ev_data;  
  switch(ev) 
  {
    case NS_HTTP_REQUEST:
    {
      mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
      if(mg_vcmp(&hm->uri, "/read") == 0)
      {
        // mg_printf_http_chunk(nc, "{ \"result\": %lf }", (*t100_list_ptr)[0].getThermocoupleTemperature());        
        mg_printf_http_chunk(nc, "{\n");
        mg_printf_http_chunk(nc, "  \"deviceCount\": %d,\n",t100_count);
        mg_printf_http_chunk(nc, "  \"readings\": [\n");
        for(int i=0; i<t100_count; i++)
        {          
          mg_printf_http_chunk(nc, "    {\n");          
          mtx.lock();
          mg_printf_http_chunk(nc, "      \"ID\": %d,\n", t100_list.at(i)->getMySerialNumber());
          mg_printf_http_chunk(nc, "      \"TC\": %f,\n", t100_list.at(i)->getThermocoupleTemperature());
          mg_printf_http_chunk(nc, "      \"CJ\": %f\n", t100_list.at(i)->getColdJunctionTemperature());          
          mtx.unlock();
          if(i < (t100_count - 1))
          {
            mg_printf_http_chunk(nc, "    },\n");
          }
          else
          {
            mg_printf_http_chunk(nc, "    }\n");
          }
        }            
        mg_printf_http_chunk(nc, "  ]\n");
        mg_printf_http_chunk(nc, "}\n");
      }
      else if(mg_vcmp(&hm->uri, "/search") == 0)
      {        
        /* Do the scan ... */
        mtx.lock();
        t100Helper_fillDeviceList(t100_list);
        mtx.unlock();
        
        /* Report back the count */
        mg_printf_http_chunk(nc, "{\n");
        mg_printf_http_chunk(nc, "  \"deviceCount\": %d\n",t100_count);
        mg_printf_http_chunk(nc, "}\n");
      }         
      mg_send_http_chunk(nc, "", 0);  /* Send empty chunk, the end of response */
      break;
    }    
    default:
    {
      break;
    }
  }
}
/*---------------------------------------------------------------------------*/
void *connectionHandler(void *arg)
{
  printf("> This is the thread talking!\n");

  while(1)
  {
    mg_mgr_poll((mg_mgr*)arg, 1000);
  }

}
/*---------------------------------------------------------------------------*/