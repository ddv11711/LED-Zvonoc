void SSDP_init(void){
     //SSDP дескриптор
     HTTP.on ("/description.xml",HTTP_GET,[](){
             SSDP.schema(HTTP.client());
     });
     SSDP.setDeviceType("upnp:rootdevice");//картинка устройства
     SSDP.setSchemaURL("description.xml");//адресс дериктории
     SSDP.setHTTPPort(HTTP_PORT);//порт устройства
     SSDP.setName(SSDP_Name);//имя устройства 
     SSDP.setURL("/");//основная страница
     SSDP.setModelName("1");//модель
     SSDP.setModelNumber("000000001");//номер устройства
     SSDP.setManufacturer("DDV");//производитель
     SSDP.begin();//запуск протакола
}
