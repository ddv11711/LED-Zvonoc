void WIFI_init(){
     WiFi.mode(WIFI_STA);//режим подключения к WI-FI
     byte tries=11; //количество попыток подключения
     Serial.print("ssid:");Serial.print(_ssid);Serial.print(" password:");Serial.println(_password);
   //  WiFi.config(ip_take,dns_take,IPAddress(255,255,255,0));
     WiFi.begin(_ssid.c_str(),_password.c_str());// подключаемся к WI-FI
     while (--tries && WiFi.status()!=WL_CONNECTED){
           Serial.print(".");
           delay(1000);
     }
     Serial.println("");
     if (WiFi.status()== WL_NO_SSID_AVAIL){ip="Сеть не найдена";}
     if (WiFi.status()==6){ip="Неправильный пароль";}
     if (WiFi.status()!=WL_CONNECTED){//если не удалось подключится подымаем точку доступа
        Serial.println("WI-FI up AP");
        Serial.println(apIP);
        StartAPMode();
     }
     else { //если удалось подключится то выводим IP
          Serial.println("WI-FI connected");
          Serial.print ("IP address: ");
          Serial.println(WiFi.localIP());
          ip=WiFi.localIP().toString();
          /*dnsServer.setTTL(300);
          dnsServer.start(DNS_PORT,"zvonoc",WiFi.localIP());*/
     }
}
bool StartAPMode(){
     WiFi.disconnect();
     WiFi.mode(WIFI_AP);//переходи в режим точки доступа
     WiFi.softAPConfig(apIP,apIP,IPAddress(255,255,255,0));
     WiFi.softAP(_ssidAP.c_str(),_passwordAP.c_str());
     dnsServer.start(DNS_PORT,"*",apIP);
     return true;
}
